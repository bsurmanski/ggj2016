#version 300 es
//#extension GL_ARB_explicit_attrib_location : require
precision highp float;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;

in vec3 position;
in vec3 normal;
in vec2 uv;

smooth out vec4 fnormal;
smooth out vec4 fposition;
smooth out vec2 fuv;

void main()
{
    gl_Position = mvpMatrix * vec4(position, 1.0f);
    fposition = mMatrix * vec4(position, 1.0f);
    fnormal = mMatrix * vec4(normal, 0.0f);
    fuv = vec2(uv.x, -uv.y); // uv.y is inversed
}
