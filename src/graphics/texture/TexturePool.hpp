#pragma once
#include <atomic>
#include <algorithm>
#include <cstring>
#include <limits>
#include <thread>
#include <vector>
#include <bgfx/bgfx.h>
#include <boost/unordered/unordered_flat_map.hpp>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "../util/concepts.hpp"
#include "../util/os/filesystem.hpp"

namespace TexturePool {
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef u32 TextureID;
    using std::atomic, std::vector, std::move, std::numeric_limits, std::memory_order_relaxed, std::thread, boost::unordered_flat_map, bgfx::UniformHandle, bgfx::TextureHandle, bgfx::createTexture2D, bgfx::createUniform, bgfx::setTexture, bgfx::setUniform, bgfx::copy, bgfx::updateTexture2D, bgfx::UniformType, bgfx::TextureFormat, bgfx::destroy, Util::FilePath, Util::OS::readFile;

    namespace detail {
        struct TextureInfo {
            i32 height{0};
            i32 width{0};
            float u0{0.0f};
            float v0{0.0f};
            float u1{1.0f};
            float v1{1.0f};
        };

        struct AtlasRect {
            u16 x{0};
            u16 y{0};
            u16 w{0};
            u16 h{0};
        };

        struct TextureRecord {
            i32 width{0};
            i32 height{0};
            AtlasRect allocation{};
            AtlasRect content{};
            float u0{0.0f};
            float v0{0.0f};
            float u1{1.0f};
            float v1{1.0f};
            vector<u8> pixels;
        };

        struct PackItem {
            TextureID id{0};
            u16 w{0};
            u16 h{0};
            bool placeholder{false};
        };

        inline constexpr TextureID MISSING_TEXTURE_ID = 0;
        inline constexpr u16 SLOT_PADDING = 1;
        inline constexpr u16 INITIAL_ATLAS_EDGE = 1024;

        inline unordered_flat_map<TextureID, TextureRecord> registry;
        inline atomic<TextureID> nextId{0};
        inline TextureHandle atlasTexture = BGFX_INVALID_HANDLE;
        inline UniformHandle sampler = BGFX_INVALID_HANDLE;
        inline UniformHandle uvRectUniform = BGFX_INVALID_HANDLE;
        inline vector<AtlasRect> freeRects;
        inline u16 atlasWidth{0}, atlasHeight{0}, atlasMaxEdge{0};

        [[nodiscard]] inline TextureID fallbackMissingTexture() noexcept {
            if (registry.find(MISSING_TEXTURE_ID) != registry.end()) return MISSING_TEXTURE_ID;
            lerr <<
                "[TexturePool] Missing texture is missing!"
                "This is a critical error that will likely cause crashes everywhere."
                "Exiting immediately." << endl;
            Fatal::exit(Fatal::TEXTUREPOOL_MISSING_MISSING_PNG);
        }

        [[nodiscard]] inline bool rectContains(const AtlasRect& a, const AtlasRect& b) noexcept {
            return
                a.x <= b.x
             && a.y <= b.y
             && static_cast<u32>(a.x) + static_cast<u32>(a.w) >= static_cast<u32>(b.x) + static_cast<u32>(b.w)
             && static_cast<u32>(a.y) + static_cast<u32>(a.h) >= static_cast<u32>(b.y) + static_cast<u32>(b.h);
        }
    }

    static void pruneAndMergeFreeRects(vector<AtlasRect>& rects) noexcept {
        rects.erase(std::remove_if(rects.begin(), rects.end(), [](const AtlasRect& rect) {
            return rect.w == 0 || rect.h == 0;
        }), rects.end());

        for (size_t i = 0; i < rects.size(); i++) {
            for (size_t j = i + 1; j < rects.size();) {
                if (rectContains(rects[i], rects[j])) {
                    rects.erase(rects.begin() + static_cast<i32>(j));
                    continue;
                }
                if (rectContains(rects[j], rects[i])) {
                    rects.erase(rects.begin() + static_cast<i32>(i));
                    i = 0;
                    j = 0;
                    continue;
                }
                j++;
            }
        }

        bool merged = true;
        while (merged) {
            merged = false;
            for (size_t i = 0; i < rects.size() && !merged; i++) {
                for (size_t j = i + 1; j < rects.size(); j++) {
                    auto& a = rects[i];
                    auto& b = rects[j];
                    if (a.y == b.y && a.h == b.h) {
                        if (static_cast<u32>(a.x) + static_cast<u32>(a.w) == b.x) {
                            a.w = static_cast<u16>(a.w + b.w);
                            rects.erase(rects.begin() + static_cast<i32>(j));
                            merged = true;
                            break;
                        }
                        if (static_cast<u32>(b.x) + static_cast<u32>(b.w) == a.x) {
                            a.x = b.x;
                            a.w = static_cast<u16>(a.w + b.w);
                            rects.erase(rects.begin() + static_cast<i32>(j));
                            merged = true;
                            break;
                        }
                    }
                    if (a.x == b.x && a.w == b.w) {
                        if (static_cast<u32>(a.y) + static_cast<u32>(a.h) == b.y) {
                            a.h = static_cast<u16>(a.h + b.h);
                            rects.erase(rects.begin() + static_cast<i32>(j));
                            merged = true;
                            break;
                        }
                        if (static_cast<u32>(b.y) + static_cast<u32>(b.h) == a.y) {
                            a.y = b.y;
                            a.h = static_cast<u16>(a.h + b.h);
                            rects.erase(rects.begin() + static_cast<i32>(j));
                            merged = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    static void addFreeRect(vector<AtlasRect>& rects, AtlasRect rect) noexcept {
        if (rect.w == 0 || rect.h == 0) return;
        rects.emplace_back(rect);
        pruneAndMergeFreeRects(rects);
    }

    [[nodiscard]] static bool allocateRect(vector<AtlasRect>& rects, u16 w, u16 h, AtlasRect& outRect) noexcept {
        i32 bestIndex = -1;
        u32 bestWaste = numeric_limits<u32>::max();
        u32 bestShortSide = numeric_limits<u32>::max();

        for (i32 i = 0; i < static_cast<i32>(rects.size()); i++) {
            const auto& candidate = rects[static_cast<size_t>(i)];
            if (candidate.w < w || candidate.h < h) continue;

            const u32 waste = static_cast<u32>(candidate.w - w) * static_cast<u32>(candidate.h - h);
            const u32 shortSide = std::min(static_cast<u32>(candidate.w - w), static_cast<u32>(candidate.h - h));
            if (waste < bestWaste || (waste == bestWaste && shortSide < bestShortSide)) {
                bestIndex = i;
                bestWaste = waste;
                bestShortSide = shortSide;
            }
        }

        if (bestIndex < 0) return false;

        const AtlasRect chosen = rects[static_cast<size_t>(bestIndex)];
        rects.erase(rects.begin() + bestIndex);
        outRect = AtlasRect { chosen.x, chosen.y, w, h };

        const u16 leftoverW = static_cast<u16>(chosen.w - w);
        const u16 leftoverH = static_cast<u16>(chosen.h - h);
        if (leftoverW > leftoverH) {
            addFreeRect(rects, AtlasRect {
                static_cast<u16>(outRect.x + w), outRect.y, leftoverW, chosen.h
            });
            addFreeRect(rects, AtlasRect {
                outRect.x, static_cast<u16>(outRect.y + h), w, leftoverH
            });
        }
        else {
            addFreeRect(rects, AtlasRect {
                static_cast<u16>(outRect.x + w), outRect.y, leftoverW, h
            });
            addFreeRect(rects, AtlasRect {
                outRect.x, static_cast<u16>(outRect.y + h), chosen.w, leftoverH
            });
        }

        pruneAndMergeFreeRects(rects);
        return true;
    }

    [[nodiscard]] static bool surfaceToPixelsRGBA8(const SDL_Surface* source, i32& outWidth, i32& outHeight, vector<u8>& outPixels) noexcept {
        auto* formatted = SDL_ConvertSurface(source, SDL_PIXELFORMAT_ABGR8888);
        if (formatted == nullptr) return false;

        const bool mustLock = SDL_MUSTLOCK(formatted);
        if (mustLock && !SDL_LockSurface(formatted)) {
            SDL_DestroySurface(formatted);
            return false;
        }

        outWidth = formatted->w;
        outHeight = formatted->h;
        if (outWidth <= 0 || outHeight <= 0) {
            if (mustLock) SDL_UnlockSurface(formatted);
            SDL_DestroySurface(formatted);
            return false;
        }

        const size_t rowBytes = static_cast<size_t>(outWidth) * 4ull;
        outPixels.resize(rowBytes * static_cast<size_t>(outHeight));
        const auto* src = static_cast<const u8*>(formatted->pixels);
        for (i32 y = 0; y < outHeight; y++) {
            std::memcpy(
                outPixels.data() + rowBytes * static_cast<size_t>(y),
                src + static_cast<size_t>(formatted->pitch) * static_cast<size_t>(y),
                rowBytes
            );
        }

        if (mustLock) SDL_UnlockSurface(formatted);
        SDL_DestroySurface(formatted);
        return true;
    }

    [[nodiscard]] static vector<u8> buildPaddedPixels(const TextureRecord& record) noexcept {
        vector<u8> result(static_cast<size_t>(record.allocation.w) * static_cast<size_t>(record.allocation.h) * 4ull);
        for (u16 y = 0; y < record.allocation.h; y++) {
            const i32 srcY = std::clamp(static_cast<i32>(y) - static_cast<i32>(SLOT_PADDING), 0, record.height - 1);
            for (u16 x = 0; x < record.allocation.w; x++) {
                const i32 srcX = std::clamp(static_cast<i32>(x) - static_cast<i32>(SLOT_PADDING), 0, record.width - 1);
                const size_t srcOffset = (static_cast<size_t>(srcY) * static_cast<size_t>(record.width) + static_cast<size_t>(srcX)) * 4ull;
                const size_t dstOffset = (static_cast<size_t>(y) * static_cast<size_t>(record.allocation.w) + static_cast<size_t>(x)) * 4ull;
                std::memcpy(result.data() + dstOffset, record.pixels.data() + srcOffset, 4ull);
            }
        }
        return result;
    }

    [[nodiscard]] static u16 chooseInitialAtlasEdge(u16 maxEdge) noexcept {
        u16 candidate = 1;
        while (candidate < maxEdge && candidate < INITIAL_ATLAS_EDGE) candidate = static_cast<u16>(candidate << 1);
        return std::min(candidate, maxEdge);
    }

    static void setRecordPlacement(TextureRecord& record, const AtlasRect& slot) noexcept {
        record.allocation = slot;
        record.content = AtlasRect {
            static_cast<u16>(slot.x + SLOT_PADDING),
            static_cast<u16>(slot.y + SLOT_PADDING),
            static_cast<u16>(record.width),
            static_cast<u16>(record.height)
        };
        record.u0 = static_cast<float>(record.content.x) / static_cast<float>(atlasWidth);
        record.v0 = static_cast<float>(record.content.y) / static_cast<float>(atlasHeight);
        record.u1 = static_cast<float>(record.content.x + record.content.w) / static_cast<float>(atlasWidth);
        record.v1 = static_cast<float>(record.content.y + record.content.h) / static_cast<float>(atlasHeight);
    }

    static void uploadRecord(const TextureRecord& record) noexcept {
        auto paddedPixels = buildPaddedPixels(record);
        const auto* mem = copy(paddedPixels.data(), static_cast<u32>(paddedPixels.size()));
        updateTexture2D(
            atlasTexture,
            0,
            0,
            record.allocation.x,
            record.allocation.y,
            record.allocation.w,
            record.allocation.h,
            mem
        );
    }

    [[nodiscard]] static bool packItems(
        u16 width,
        u16 height,
        vector<PackItem> items,
        unordered_flat_map<TextureID, AtlasRect>& outPlacements,
        vector<AtlasRect>& outFreeRects,
        AtlasRect& outPlaceholder
    ) noexcept {
        std::sort(items.begin(), items.end(), [](const PackItem& lhs, const PackItem& rhs) {
            const u32 lhsArea = static_cast<u32>(lhs.w) * static_cast<u32>(lhs.h);
            const u32 rhsArea = static_cast<u32>(rhs.w) * static_cast<u32>(rhs.h);
            if (lhsArea != rhsArea) return lhsArea > rhsArea;
            return std::max(lhs.w, lhs.h) > std::max(rhs.w, rhs.h);
        });

        vector<AtlasRect> workFreeRects;
        workFreeRects.emplace_back(AtlasRect {0, 0, width, height});
        outPlacements.clear();
        outPlaceholder = AtlasRect {};

        for (const auto& item : items) {
            AtlasRect slot {};
            if (!allocateRect(workFreeRects, item.w, item.h, slot)) return false;
            if (item.placeholder) outPlaceholder = slot;
            else outPlacements.emplace(item.id, slot);
        }

        outFreeRects = move(workFreeRects);
        return true;
    }

    static void growCandidate(u16& width, u16& height) noexcept {
        if (width == atlasMaxEdge && height == atlasMaxEdge) return;
        if ((width <= height && width < atlasMaxEdge) || height == atlasMaxEdge) {
            width = static_cast<u16>(std::min(static_cast<u32>(atlasMaxEdge), static_cast<u32>(width) * 2u));
        }
        else {
            height = static_cast<u16>(std::min(static_cast<u32>(atlasMaxEdge), static_cast<u32>(height) * 2u));
        }
    }

    [[nodiscard]] static bool growAndRepack(u16 requiredW, u16 requiredH, AtlasRect& outReservedSlot) noexcept {
        vector<PackItem> items;
        items.reserve(registry.size() + 1);
        for (const auto& [id, record] : registry) items.emplace_back(PackItem{id, record.allocation.w, record.allocation.h, false});
        items.emplace_back(PackItem{MISSING_TEXTURE_ID, requiredW, requiredH, true});

        unordered_flat_map<TextureID, AtlasRect> placements;
        vector<AtlasRect> nextFreeRects;
        AtlasRect placeholderSlot {};

        u16 candidateW = atlasWidth;
        u16 candidateH = atlasHeight;
        bool packed = false;
        while (true) {
            if (candidateW >= requiredW && candidateH >= requiredH && packItems(candidateW, candidateH, items, placements, nextFreeRects, placeholderSlot)) {
                packed = true;
                break;
            }

            if (candidateW == atlasMaxEdge && candidateH == atlasMaxEdge) break;
            growCandidate(candidateW, candidateH);
        }

        if (!packed) return false;

        const auto nextAtlas = createTexture2D(
            candidateW,
            candidateH,
            false,
            1,
            TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT
        );
        if (!bgfx::isValid(nextAtlas)) return false;

        const auto oldAtlas = atlasTexture;
        atlasTexture = nextAtlas;
        atlasWidth = candidateW;
        atlasHeight = candidateH;
        freeRects = move(nextFreeRects);

        for (auto& [id, record] : registry) {
            const auto placement = placements.find(id);
            if (placement == placements.end()) continue;
            setRecordPlacement(record, placement->second);
            uploadRecord(record);
        }

        if (bgfx::isValid(oldAtlas)) destroy(oldAtlas);
        outReservedSlot = placeholderSlot;
        return true;
    }

    [[nodiscard]] static const TextureRecord* getRecord(TextureID id) noexcept {
        auto p = registry.find(id);
        if (p != registry.end()) return &p->second;
        p = registry.find(MISSING_TEXTURE_ID);
        if (p != registry.end()) return &p->second;
        return nullptr;
    }

    [[nodiscard]] static TextureID addTextureFromPixels(i32 width, i32 height, vector<u8>&& pixels) noexcept {
        if (width <= 0 || height <= 0) {
            lerr << "[TexturePool] Texture dimensions must be positive." << endl;
            return fallbackMissingTexture();
        }
        if (width > numeric_limits<i16>::max() || height > numeric_limits<i16>::max()) {
            lerr << "[TexturePool] Texture dimensions overflow 16-bit limits." << endl;
            return fallbackMissingTexture();
        }
        if (width > atlasMaxEdge || height > atlasMaxEdge) {
            lerr << "[TexturePool] Texture is larger than the current device texture cap: " << width << "x" << height << endl;
            return fallbackMissingTexture();
        }
        if (pixels.empty()) {
            lerr << "[TexturePool] Texture decode returned an empty pixel buffer." << endl;
            return fallbackMissingTexture();
        }

        const u32 paddedW = static_cast<u32>(width) + static_cast<u32>(SLOT_PADDING) * 2u;
        const u32 paddedH = static_cast<u32>(height) + static_cast<u32>(SLOT_PADDING) * 2u;
        if (paddedW > atlasMaxEdge || paddedH > atlasMaxEdge) {
            lerr << "[TexturePool] Texture is too large after atlas padding: " << width << "x" << height << endl;
            return fallbackMissingTexture();
        }

        AtlasRect slot {};
        if (!allocateRect(freeRects, static_cast<u16>(paddedW), static_cast<u16>(paddedH), slot)) {
            if (!growAndRepack(static_cast<u16>(paddedW), static_cast<u16>(paddedH), slot)) {
                lerr << "[TexturePool] Atlas is full and cannot grow any further." << endl;
                return fallbackMissingTexture();
            }
        }

        TextureRecord record {};
        record.width = width;
        record.height = height;
        record.pixels = move(pixels);
        setRecordPlacement(record, slot);

        const TextureID id = nextId.fetch_add(1, memory_order_relaxed);
        const auto [pos, inserted] = registry.emplace(id, move(record));
        if (!inserted) {
            addFreeRect(freeRects, slot);
            lerr << "[TexturePool] Failed to register texture id " << id << endl;
            return fallbackMissingTexture();
        }

        uploadRecord(pos->second);
        return id;
    }

    template <bool physfs, FilePath PathType>
    TextureID addTexture2(PathType&& path_) noexcept {
        vector<u8> fileData;
        if (!readFile<physfs>(path_, fileData) || fileData.empty()) {
            lerr << "[TexturePool] Failed to read texture file " << path_ << endl;
            return fallbackMissingTexture();
        }

        auto* stream = SDL_IOFromConstMem(fileData.data(), fileData.size());
        if (stream == nullptr) {
            lerr << "[TexturePool] Failed to create SDL stream for texture " << path_ << ": " << SDL_GetError() << endl;
            return fallbackMissingTexture();
        }

        auto* imgData = IMG_Load_IO(stream, true);
        if (imgData == nullptr) {
            lerr << "[TexturePool] Failed to decode texture " << path_ << ": " << SDL_GetError() << endl;
            return fallbackMissingTexture();
        }

        i32 width = 0, height = 0;
        vector<u8> pixels;
        if (!surfaceToPixelsRGBA8(imgData, width, height, pixels)) {
            SDL_DestroySurface(imgData);
            lerr << "[TexturePool] Failed to convert texture to RGBA8 " << path_ << endl;
            return fallbackMissingTexture();
        }

        SDL_DestroySurface(imgData);
        return addTextureFromPixels(width, height, move(pixels));
    }

    void init(const char* samplerName) noexcept {
        registry.clear();
        freeRects.clear();
        nextId.store(0, memory_order_relaxed);

        sampler = createUniform(samplerName, UniformType::Sampler);
        uvRectUniform = createUniform("u_uvRect", UniformType::Vec4);

        const auto* caps = bgfx::getCaps();
        if (caps == nullptr) {
            lerr << "[TexturePool] Failed to query bgfx caps." << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FAILED);
        }
        atlasMaxEdge = static_cast<u16>(std::min(static_cast<u32>(caps->limits.maxTextureSize), static_cast<u32>(numeric_limits<u16>::max())));
        if (atlasMaxEdge == 0) {
            lerr << "[TexturePool] Invalid max texture size from bgfx caps." << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FAILED);
        }

        const u16 initialEdge = chooseInitialAtlasEdge(atlasMaxEdge);
        atlasTexture = createTexture2D(
            initialEdge,
            initialEdge,
            false,
            1,
            TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT
        );
        if (!bgfx::isValid(atlasTexture)) {
            lerr << "[TexturePool] Failed to create atlas texture." << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FAILED);
        }

        atlasWidth = initialEdge;
        atlasHeight = initialEdge;
        freeRects.emplace_back(AtlasRect {0, 0, atlasWidth, atlasHeight});

        if (addTexture2<false>("assets/textures/missing.png") != MISSING_TEXTURE_ID) {
            lerr << "[TexturePool] Missing texture must be registered as ID 0." << endl;
            Fatal::exit(Fatal::TEXTUREPOOL_MISSING_MISSING_PNG);
        }
    }

    void shutdown() noexcept {
        registry.clear();
        freeRects.clear();
        nextId.store(0, memory_order_relaxed);
        atlasWidth = 0;
        atlasHeight = 0;
        atlasMaxEdge = 0;

        if (bgfx::isValid(atlasTexture)) destroy(atlasTexture);
        if (bgfx::isValid(uvRectUniform)) destroy(uvRectUniform);
        if (bgfx::isValid(sampler)) destroy(sampler);

        atlasTexture = BGFX_INVALID_HANDLE;
        uvRectUniform = BGFX_INVALID_HANDLE;
        sampler = BGFX_INVALID_HANDLE;
    }

    void useTexture(TextureID id, u8 textureDataIndex) noexcept {
        useTexture(id, 0.0f, 0.0f, 1.0f, 1.0f, textureDataIndex);
    }

    void useTexture(TextureID id, float localU0, float localV0, float localU1, float localV1, u8 textureDataIndex) noexcept {
        const auto* record = getRecord(id);
        if (record == nullptr) return;
        if (!bgfx::isValid(atlasTexture) || !bgfx::isValid(sampler) || !bgfx::isValid(uvRectUniform)) return;

        const float du = record->u1 - record->u0;
        const float dv = record->v1 - record->v0;
        const float uvRect[4] {
            record->u0 + localU0 * du,
            record->v0 + localV0 * dv,
            record->u0 + localU1 * du,
            record->v0 + localV1 * dv
        };
        setUniform(uvRectUniform, uvRect);
        setTexture(textureDataIndex, sampler, atlasTexture);
    }

    bool getTextureInfo(TextureID id, TextureInfo& result) noexcept {
        const auto p = registry.find(id);
        if (p == registry.end()) return false;

        result.width = p->second.width;
        result.height = p->second.height;
        result.u0 = p->second.u0;
        result.v0 = p->second.v0;
        result.u1 = p->second.u1;
        result.v1 = p->second.v1;
        return true;
    }

    void removeTexture(TextureID id) noexcept {
        if (id == MISSING_TEXTURE_ID) return;

        const auto p = registry.find(id);
        if (p == registry.end()) return;

        addFreeRect(freeRects, p->second.allocation);
        registry.erase(p);
    }
}
