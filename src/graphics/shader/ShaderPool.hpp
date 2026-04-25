#pragma once
#include <bgfx/bgfx.h>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "../../util/SlotTable.hpp"
#include "ShaderDef.hpp"

namespace ShaderPool {
    typedef uint8_t u8;
    using Util::SlotTable;

    MAKE_DISTINCT_HANDLE(ShaderPoolHandle);

    namespace detail {
        inline SlotTable<bgfx::ProgramHandle, ShaderPoolHandle> registry;

        //This is a critical function so we always panic if it fails.
        [[nodiscard]] inline bgfx::ShaderHandle loadShader(const ShaderDef& shaderDef) noexcept {
            const u8* data = nullptr;
            size_t size = 0;
            switch (bgfx::getRendererType()) {
                using enum bgfx::RendererType::Enum;

                case Direct3D11:
                    data = shaderDef.dx11;
                    size = shaderDef.dx11_size;
                    break;
                case Direct3D12:
                    data = shaderDef.dx12;
                    size = shaderDef.dx12_size;
                    break;
                case Metal:
                    data = shaderDef.metal;
                    size = shaderDef.metal_size;
                    break;
                case OpenGL:
                    data = shaderDef.opengl;
                    size = shaderDef.opengl_size;
                    break;
                case OpenGLES:
                    data = shaderDef.opengles;
                    size = shaderDef.opengles_size;
                    break;
                case Vulkan:
                    data = shaderDef.vulkan;
                    size = shaderDef.vulkan_size;
                    break;
                default:
                    lerr << "[ShaderPool] No valid render backends!" << endl;
                    Fatal::exit(Fatal::BGFX_NO_VALID_RENDER_BACKEND);
            }
        #if CG_DEBUG
            if (data == nullptr || size == 0) {
                lerr << "[ShaderPool] Failed to load shader!" << endl;
                Fatal::exit(Fatal::BGFX_CREATE_SHADER_FAILED);
            }
        #endif
            const bgfx::Memory* memory = bgfx::alloc(size + 1);
            memory->data[memory->size - 1] = '\0';
            const auto handle = bgfx::createShader(memory);
            if (!bgfx::isValid(handle)) {
                lerr << "[ShaderPool] Failed to create shader!" << endl;
                Fatal::exit(Fatal::BGFX_CREATE_SHADER_FAILED);
            }
            return handle;
        }

        [[nodiscard]] inline bgfx::ProgramHandle loadShaderSet(const ShaderSetDef& shaderSetDef) noexcept {
            const auto vsh = loadShader(shaderSetDef.vs), fsh = loadShader(shaderSetDef.fs);
            const auto handle = bgfx::createProgram(vsh, fsh, true);
            if (!bgfx::isValid(handle)) {
                lerr << "[ShaderPool] Failed to create shader program!" << endl;
                Fatal::exit(Fatal::BGFX_CREATE_SHADER_FAILED);
            }
            return handle;
        }
    }

    inline void init() noexcept {}

    inline void shutdown() noexcept {
        for (const auto& programHandle : detail::registry) bgfx::destroy(programHandle);
    }

    [[nodiscard]] inline ShaderPoolHandle addShader(const ShaderSetDef& shaderSetDef) noexcept {
        const auto programHandle = detail::loadShaderSet(shaderSetDef);
        return detail::registry.emplace(programHandle);
    }

    [[nodiscard]] inline bgfx::ProgramHandle getShader(ShaderPoolHandle handle) noexcept {
        const auto* p = detail::registry.get(handle);
        if (p == nullptr) {
            lerr << "[ShaderPool] Failed to get shader " << handle << "!" << endl;
            Fatal::exit(Fatal::BGFX_GET_SHADER_FAILED);
        }
        return *p;
    }

    [[nodiscard]] inline bool removeShader(ShaderPoolHandle handle) noexcept {
        const auto* p = detail::registry.get(handle);
        if (p == nullptr) return false;
        bgfx::destroy(*p);
        static_cast<void>(detail::registry.destroy(handle));
        return true;
    }
}