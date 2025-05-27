#include <atomic>
#include <limits>
#include <optional>
#include <unordered_map>
#include <bgfx/bgfx.h>
#include <SDL3_image/SDL_image.h>

#include "../debug/Logger.hpp"
#include "TexturePool.hpp"

//MASSIVE todo: store texture in one texture atlas and use textureId to index coordinates.
//as well as dynamically managing texture size, allowing registering 32x32 or bigger texture.
namespace TexturePool {
    typedef int16_t i16;
    using std::atomic, std::unordered_map, bgfx::UniformHandle, bgfx::createUniform, bgfx::destroy, std::optional, std::move, std::numeric_limits, std::memory_order_relaxed;
    TextureID addTexture(const char* filePath, bool noVerticalFilp) noexcept;

    unordered_map<TextureID, Texture> registry;
    atomic<TextureID> nextId(0);
    UniformHandle sampler;

    void init(const char* samplerName) noexcept {
        sampler = createUniform(samplerName, bgfx::UniformType::Sampler);
        addTexture("assets/textures/missing.png");
    }

    void shutdown() noexcept {
        for (const auto& [id, texture] : registry) {
            destroy(texture.handle);
            SDL_DestroySurface(const_cast<SDL_Surface*>(texture.data));
        }
        destroy(sampler);
    }

    TextureID addTexture(const char* filePath, bool noVerticalFilp) noexcept {
        Texture texture{};
        auto* imgData = IMG_Load(filePath);
        if (SDL_MUSTLOCK(imgData) && SDL_LockSurface(imgData) < 0) {
            lerr << "[TexturePool] Texture from file " << filePath << " is not lockable!" << endl;
            return MISSING_TEXTURE_ID;
        }
        if (imgData->h > numeric_limits<i16>::max()) {
            lerr << "Texture height overflow (highest " << numeric_limits<i16>::max() << "): " << filePath << endl;
            return 0;
        }
        if (imgData->w > numeric_limits<i16>::max()) {
            lerr << "Texture width overflow (highest " << numeric_limits<i16>::max() << "): " << filePath << endl;
            return 0;
        }
        auto* formattedData = SDL_ConvertSurface(imgData, SDL_PIXELFORMAT_RGBA8888);
        if (formattedData == nullptr) {
            lerr << "[TexturePool] Texture from file " << filePath << " is not convertable to RGBA8 format!" << endl;
            return MISSING_TEXTURE_ID;
        }
        SDL_DestroySurface(imgData);
        //todo: Build texture atlas for only 16x16 textures (consider dynamic-sized atlas?)
        texture.handle = bgfx::createTexture2D(
            imgData->w,
            imgData->h,
            false, 1,
            bgfx::TextureFormat::RGBA8,
            //Temporary: Disable pixel interpolation
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
            bgfx::makeRef(imgData->pixels, imgData->w * imgData->h * 4)
        );
        registry.emplace(nextId, move(texture));
        auto id = nextId.fetch_add(1, memory_order_relaxed);
        return id;
    }

    void useTexture(TextureID id, u8 textureDataIndex) noexcept {
        auto p = registry.find(id);
        //Fallback to a missing texture. See `init()`.
        if (p == registry.end()) bgfx::setTexture(textureDataIndex, sampler, registry[0].handle);
        else bgfx::setTexture(textureDataIndex, sampler, p->second.handle);
    }

    const Texture* getTexture(TextureID id) noexcept {
        auto p = registry.find(id);
        if (p == registry.end()) return nullptr;
        return &(p->second);
    }

    void removeTexture(TextureID id) noexcept {
        auto p = registry.find(id);
        if (p == registry.end()) return;
        destroy(p->second.handle);
        SDL_DestroySurface(p->second.data);
        registry.erase(p);
    }
}