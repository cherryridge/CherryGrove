$input v_texcoord0
#include "../../libs/bgfx_shader/bgfx_shader.sh"
#include "../../libs/bgfx_shader/shaderlib.sh"

SAMPLER2D(s_texture, 0);

void main(){
	vec4 color = texture2D(s_texture, v_texcoord0);
	gl_FragColor = color;
}