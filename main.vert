#version 330 core
layout (location = 0) in vec2 aPosition;
in vec4 aColor;
uniform mat4 uMatrix;
uniform mat4 uModelViewMatrix;
out vec4 vColor;

void main() {
    vColor = vec4(1.f, 0.f, 0.f, 0.5f);
//    vColor = aColor;
    vec4 pos = vec4(aPosition.x, aPosition.y, 0.0f, 1.f);
    gl_Position = uMatrix * (uModelViewMatrix * pos);
}
