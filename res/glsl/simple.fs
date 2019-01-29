#version 330

in vec2 texco;
uniform sampler2D tex;

out vec4 color;

void main(void) {
    vec4 c = texture(tex, texco);
    if(c.a < 0.1) discard;
    color = c;
}
