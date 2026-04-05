#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../../debug/Logger.hpp"

namespace TexturePool::detail {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::string, std::vector;

    //RGBA32 is guaranteed in this struct.
    struct LoneTexture {
        u32 width, height;
        vector<u8> pixels;
    };

    [[nodiscard]] inline bool loadTexture(const vector<u8>& mem, LoneTexture& result, const string& path_ = string{}) noexcept {
        SDL_IOStream* stream = SDL_IOFromConstMem(mem.data(), mem.size());
        if (stream == nullptr) {
            lerr << "[TexturePool] Failed to create SDL stream for texture " << path_ << ": " << SDL_GetError() << endl;
            return false;
        }
        SDL_Surface* surface = IMG_Load_IO(stream, true);
        if (surface == nullptr) {
            lerr << "[TexturePool] Failed to decode texture " << path_ << ": " << SDL_GetError() << endl;
            return false;
        }
        if (SDL_MUSTLOCK(surface) && !SDL_LockSurface(surface)) {
            lerr << "[TexturePool] Texture " << path_ << " needs to be locked but failed to do so: " << SDL_GetError() << endl;
            SDL_DestroySurface(surface);
            return false;
        }
        result.width = surface->w;
        result.height = surface->h;
        if (surface->format == SDL_PIXELFORMAT_RGBA32) {
            result.pixels.resize(static_cast<u64>(surface->w) * static_cast<u64>(surface->h) * 4ull);
            result.pixels.assign(
                reinterpret_cast<u8*>(surface->pixels),
                reinterpret_cast<u8*>(surface->pixels) + static_cast<u64>(surface->w) * static_cast<u64>(surface->h) * 4ull
            );
        }
        else {
            SDL_Surface* formatted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
            if (formatted == nullptr) {
                lerr << "[TexturePool] Failed to convert texture " << path_ << " to RGBA8 format: " << SDL_GetError() << endl;
                if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
                SDL_DestroySurface(surface);
                return false;
            }
            if (SDL_MUSTLOCK(formatted) && !SDL_LockSurface(formatted)) {
                lerr << "[TexturePool] Converted texture " << path_ << " needs to be locked but failed to do so: " << SDL_GetError() << endl;
                SDL_DestroySurface(surface);
                SDL_DestroySurface(formatted);
                return false;
            }
            result.pixels.resize(static_cast<u64>(formatted->w) * static_cast<u64>(formatted->h) * 4ull);
            result.pixels.assign(
                reinterpret_cast<u8*>(formatted->pixels),
                reinterpret_cast<u8*>(formatted->pixels) + static_cast<u64>(formatted->w) * static_cast<u64>(formatted->h) * 4ull
            );
            if (SDL_MUSTLOCK(formatted)) SDL_UnlockSurface(formatted);
            SDL_DestroySurface(formatted);
        }
        if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        SDL_DestroySurface(surface);
        return true;
    }
}