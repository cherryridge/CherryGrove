$input v_texcoord0
#include "../../lib_archive/source/bgfx/src/bgfx_shader.sh"
#include "../../lib_archive/source/bgfx/examples/common/shaderlib.sh"

SAMPLER2D(s_texture, 0);

void main(){
	vec4 color = texture2D(s_texture, v_texcoord0);
	gl_FragColor = color;
}