#pragma once
#include <bgfx/bgfx.h>
#include <boost/unordered/unordered_flat_map.hpp>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "ShaderDef.hpp"

namespace ShaderPool {
    typedef u8 u8;
    typedef uint16_t ShaderID;
    using bgfx::RendererType, bgfx::ShaderHandle, bgfx::ProgramHandle, bgfx::destroy, boost::unordered_flat_map;

    namespace detail {
        inline ShaderID nextId;
        inline unordered_flat_map<ShaderID, ProgramHandle> registry;

        //This is a critical function so we always panic if it fails.
        inline ShaderHandle loadShader(const ShaderDef& shaderDef) noexcept {
            const u8* data = nullptr;
            size_t size = 0;
            switch (bgfx::getRendererType()) {
                case RendererType::Direct3D11:
                    data = shaderDef.dx11;
                    size = shaderDef.dx11_size;
                    break;
                case RendererType::Direct3D12:
                    data = shaderDef.dx12;
                    size = shaderDef.dx12_size;
                    break;
                case RendererType::Metal:
                    data = shaderDef.metal;
                    size = shaderDef.metal_size;
                    break;
                case RendererType::OpenGL:
                    data = shaderDef.opengl;
                    size = shaderDef.opengl_size;
                    break;
                case RendererType::OpenGLES:
                    data = shaderDef.opengles;
                    size = shaderDef.opengles_size;
                    break;
                case RendererType::Vulkan:
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

        inline ProgramHandle loadShaderSet(const ShaderSetDef& shaderSetDef) noexcept {
            const auto vsh = loadShader(shaderSetDef.vs), fsh = loadShader(shaderSetDef.fs);
            const auto handle = bgfx::createProgram(vsh, fsh, true);
            if (!bgfx::isValid(handle)) {
                lerr << "[ShaderPool] Failed to create shader program!" << endl;
                Fatal::exit(Fatal::BGFX_CREATE_SHADER_FAILED);
            }
            return handle;
        }
    }

    inline void init() noexcept {
        //Shader ID `0` is for **NOT RENDERING THE OBJECT**!
        detail::nextId = 1;
    }

    inline void shutdown() noexcept {
        for (const auto& shader : detail::registry) destroy(shader.second);
        detail::registry.clear();
    }

    [[nodiscard]] inline ShaderID addShader(const ShaderSetDef& shaderSetDef) noexcept {
        const auto handle = detail::loadShaderSet(shaderSetDef);
        const auto id = detail::nextId;
        detail::nextId++;
        detail::registry.emplace(id, handle);
        return id;
    }

    [[nodiscard]] inline const ProgramHandle& getShader(ShaderID shaderId) noexcept {
        const auto p = detail::registry.find(shaderId);
        if (p == detail::registry.end()) {
            lerr << "[ShaderPool] Failed to get shader " << shaderId << "!" << endl;
            Fatal::exit(Fatal::BGFX_GET_SHADER_FAILED);
        }
        return p->second;
    }

    [[nodiscard]] inline bool removeShader(ShaderID id) noexcept {
        const auto p = detail::registry.find(id);
        if (p == detail::registry.end()) return false;
        destroy(p->second);
        detail::registry.erase(p);
        return true;
    }
}