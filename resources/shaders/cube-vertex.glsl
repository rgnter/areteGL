#version 400

#pragma shader_stagevertex
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "common/shared.glsl"

layout (std140, set = 0, binding = 0) uniform UniformBuffer
{
    mat4 mvp;
    mat4 model;
    //float time;
} uniformBuffer;

layout (set = 0, location = 0) in vec3 pos;
layout (location = 0) out vec3 outColor;

void main()
{
    // mat4 mvp = pushConstants.clip * pushConstants.proj * pushConstants.view * pushConstants.model;
//    float noiseScale = 3.0f;
//    float time = /*uniformBuffer.time*/ 1.0f * .5f;
//    float effectPow = 1;
//
//    float sinTime = sin(time);
//
//    vec3 modifiedPos = pos * vec3(1, 1 + (1 + sinTime) / 2 * effectPow, 1);
//
//    vec4 finalPos = uniformBuffer.mvp * vec4(modifiedPos, 1.0);

    gl_Position = uniformBuffer.mvp * vec4(pos, 1.0);
    //    gl_Position = pushConstants.mvp * vec4(pos, 1);
    outColor = pos;
}