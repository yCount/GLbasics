#version 410 core

uniform float u_Offset; // uniform variable

in vec3 v_vertexColors;

out vec4 color;

void main() {
    color = vec4(v_vertexColors.r-u_Offset, v_vertexColors.g, v_vertexColors.b, 1.0f);
}
