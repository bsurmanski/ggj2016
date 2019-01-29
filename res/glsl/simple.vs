#version 330

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 mvp;

out vec2 texco;

void main(void) {
    gl_Position = mvp * vec4(position, 1.0f);
    texco = uv;
}
