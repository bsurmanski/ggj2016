#version 330

in vec2 fuv; 
uniform sampler2D t_day;
uniform sampler2D t_night; 

uniform vec3 eye; 
uniform vec3 sun;

out vec4 color;

void main(void) {
    vec4 c = texture(t_night, fuv); 
    color = c;
}
