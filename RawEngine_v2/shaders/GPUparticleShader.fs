#version 430

in vec2 TexCoord;
in vec4 ParticleColor;
in vec3 fNor;
in vec2 uv;
out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    vec4 tex = texture(particleTexture, uv);

    //if (tex.a < 0.01)
        //discard;


    FragColor = vec4(tex);
}