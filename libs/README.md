# Prebuilt Libraries

See https://docs.cherrygrove.dev/cg/releasing/building#grab-prebuilt-depedencies.

# Local Libraries

### Disclaimer: Local libraries may be modified. Check out the links below for original code.

### All of the local libraries need to be updated periodically.

- [ocornut/imgui](https://github.com/ocornut/imgui): `./imgui`
  - `352a7f101a07f17e357da779bb32444adb85d0fb` (`docking`)
  - Reason: `imconfig.h` needs to be modified.
- [MadLadSquad/UImGuiRendererExamples/bgfx](https://github.com/MadLadSquad/UImGuiRendererExamples/tree/master/bgfx): `./imgui/backends`
  - `b9554faae3a37172d9b3c22b582107a370dbcbef`
  - Reason: `imgui.<cpp|h>` needs to be renamed to `imgui_impl_bgfx.<cpp|h>` and the rest of source files need to be removed. Also, numerous places in the embedded shaders need to be modified.
- [bkaradzic/bgfx/src/bgfx_shaders.sh](https://github.com/bkaradzic/bgfx/blob/master/src/bgfx_shader.sh): `./bgfx_shader`
  - `4942e65bf9726b0fe4598f53a089f87eb6f6ab0a`
  - Reason: It's simpler for shader development to include it, not mandatory.
- [bkaradzic/bgfx/examples/common/shaderlib.sh](https://github.com/bkaradzic/bgfx/blob/master/examples/common/shaderlib.sh): `./bgfx_shader`
  - `4baed6e076276dd143ca7f10bc895a5dcadbcc8d`
  - Reason: It's simpler for shader development to include it, not mandatory.