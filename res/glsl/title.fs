#version 330

in vec2 fuv; 

uniform float tick;
uniform sampler2D t_title; 
uniform sampler2D t_space; 

out vec4 color;

void main(void) {
    mat2 rot1 = mat2(cos(tick), -sin(tick), sin(tick), cos(tick));
    mat2 rot2 = mat2(cos(-tick/4.0), -sin(-tick/4.0), sin(-tick/4.0), cos(-tick/4.0));

    vec2 off1 = vec2(sin(tick), cos(tick));
    vec2 off2 = vec2(sin(-tick), cos(-tick));
    vec4 space = texture(t_space, ((fuv+off1)*rot1)+vec2(tick))/2.0 +
                 texture(t_space, ((fuv+off2)*rot2)+vec2(-tick/3.0));
    color = texture(t_title, fuv) + space;
    //color = c;
}
