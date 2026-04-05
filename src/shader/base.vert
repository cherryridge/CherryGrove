$input a_position, a_texcoord0
$output v_texcoord0

#include "bgfx_shader.sh"

uniform vec4 u_uvRect;

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_texcoord0 = u_uvRect.xy + a_texcoord0 * (u_uvRect.zw - u_uvRect.xy);
}