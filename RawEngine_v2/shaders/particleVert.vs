#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 worldPos;

out vec2 TexCoord;
out vec4 ParticleColor;

void main()
{

    gl_Position = projection * view * vec4(worldPos, 1.0);

    TexCoord = aUV;
    ParticleColor = vec4(1,0,0,1);
}