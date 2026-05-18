#version 430

in vec2 TexCoord;
in vec4 ParticleColor;

out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    vec4 tex = texture(particleTexture, TexCoord);

    //if (tex.a < 0.01)
        //discard;

    FragColor = tex * ParticleColor;
}