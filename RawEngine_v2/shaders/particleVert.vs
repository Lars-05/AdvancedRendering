#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

struct Particle
{
    vec4 position;
    vec4 velocity;
    vec4 color;

    vec4 data1; // x = life, y = maxLife, z = size, w = alive
    vec4 data2;
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

uniform mat4 projection;
uniform mat4 view;
uniform vec3 worldPos;



out vec2 TexCoord;
out vec4 ParticleColor;

void main()
{
    Particle p = particles[gl_InstanceID];

    float size = p.data1.z;
    float alive = p.data1.w;

    // dead particle
    if (alive < 0.5)
    {
        gl_Position = vec4(0.0);
    }


    gl_Position = projection * view *vec4(worldPos, 1.0);

    TexCoord = aUV;
    ParticleColor = p.color;
}