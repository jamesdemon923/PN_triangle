#version 330 core

layout (location = 0) in vec4 vertexPosition_modelspace;
layout (location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
    gl_Position = P * V * M * vec4(vertexPosition_modelspace.xyz, 1.f);
}

