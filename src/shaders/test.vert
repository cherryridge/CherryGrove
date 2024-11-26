layout(location = 0) in vec2 a_position;
out vec4 gl_Position;

void main(){
    gl_Position = vec4(a_position, 0.0, 1.0);
}