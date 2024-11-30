$input a_position, a_color0
$output v_color0
#include "../../lib_archive/source/bgfx/src/bgfx_shader.sh"
#include "../../lib_archive/source/bgfx/examples/common/shaderlib.sh"

void main(){
    gl_Position = vec4(a_position, 1.0);
    v_color0 = a_color0;
}