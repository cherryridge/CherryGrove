#include <atomic>
#include <limits>
#include <optional>
#include <bgfx/bgfx.h>
#include <boost/unordered/unordered_flat_map.hpp>
#include <SDL3_image/SDL_image.h>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "TexturePool.hpp"

//MASSIVE todo: store texture in one texture atlas and use textureId to index coordinates.
//as well as dynamically managing texture size, allowing registering 32x32 or bigger texture.
namespace TexturePool {
    typedef int16_t i16;
    using std::atomic, boost::unordered::unordered_flat_map, bgfx::UniformHandle, bgfx::createTexture2D, bgfx::createUniform, bgfx::setTexture, bgfx::makeRef, bgfx::UniformType, bgfx::TextureFormat, bgfx::destroy, std::optional, std::move, std::numeric_limits, std::memory_order_relaxed;
    TextureID addTexture(const char* filePath) noexcept;

    unordered_flat_map<TextureID, Texture> registry;
    atomic<TextureID> nextId(0);
    UniformHandle sampler;

    void init(const char* samplerName) noexcept {
        sampler = createUniform(samplerName, UniformType::Sampler);
        addTexture("assets/textures/missing.png");
    }

    void shutdown() noexcept {
        for (const auto& [id, texture] : registry) {
            destroy(texture.handle);
            SDL_DestroySurface(const_cast<SDL_Surface*>(texture.data));
        }
        destroy(sampler);
    }

    TextureID addTexture(const char* filePath) noexcept {
        Texture texture{};
        auto* imgData = IMG_Load(filePath);
        if (imgData == nullptr) {
            lerr << "[TexturePool] Failed to add texture from file " << filePath << ": " << SDL_GetError() << endl;
            if (nextId.load(memory_order_relaxed) == 0) Fatal::exit(Fatal::TEXTUREPOOL_MISSING_MISSING_PNG);
            return MISSING_TEXTURE_ID;
        }
        if (SDL_MUSTLOCK(imgData) && !SDL_LockSurface(imgData)) {
            lerr << "[TexturePool] Texture from file " << filePath << " is not lockable: " << SDL_GetError() << endl;
            if (nextId.load(memory_order_relaxed) == 0) Fatal::exit(Fatal::TEXTUREPOOL_MISSING_MISSING_PNG);
            return MISSING_TEXTURE_ID;
        }
        if (imgData->h > numeric_limits<i16>::max()) {
            lerr << "[TexturePool] Texture height overflow (highest " << numeric_limits<i16>::max() << "): " << filePath << endl;
            return 0;
        }
        if (imgData->w > numeric_limits<i16>::max()) {
            lerr << "[TexturePool] Texture width overflow (highest " << numeric_limits<i16>::max() << "): " << filePath << endl;
            return 0;
        }
        auto* formattedData = SDL_ConvertSurface(imgData, SDL_PIXELFORMAT_ABGR8888);
        if (formattedData == nullptr) {
            lerr << "[TexturePool] Failed to convert texture from file " << filePath << " to RGBA8 format!" << endl;
            if (nextId.load(memory_order_relaxed) == 0) Fatal::exit(Fatal::TEXTUREPOOL_MISSING_MISSING_PNG);
            return MISSING_TEXTURE_ID;
        }
        SDL_DestroySurface(imgData);
        //todo: Build texture atlas for only 16x16 textures (consider dynamic-sized atlas?)
        texture.handle = createTexture2D(
            formattedData->w,
            formattedData->h,
            false, 1,
            TextureFormat::RGBA8,
            //Temporary: Disable pixel interpolation
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
            makeRef(formattedData->pixels, formattedData->w * formattedData->h * 4)
        );
        texture.data = move(formattedData);
        registry.emplace(nextId, move(texture));
        auto id = nextId.fetch_add(1, memory_order_relaxed);
        return id;
    }

    void useTexture(TextureID id, u8 textureDataIndex) noexcept {
        auto p = registry.find(id);
        //Fallback to a missing texture. See `init()`.
        if (p == registry.end()) setTexture(textureDataIndex, sampler, registry[0].handle);
        else setTexture(textureDataIndex, sampler, p->second.handle);
    }

    bool getTextureInfo(TextureID id, TextureInfo& result) noexcept {
        auto p = registry.find(id);
        if (p == registry.end()) return false;
        result.width = p->second.data->w;
        result.height = p->second.data->h;
        return true;
    }

    void removeTexture(TextureID id) noexcept {
        auto p = registry.find(id);
        if (p == registry.end()) return;
        destroy(p->second.handle);
        SDL_DestroySurface(p->second.data);
        registry.erase(p);
    }
}