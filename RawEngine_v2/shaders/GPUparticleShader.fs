#version 430

in vec2 TexCoord;
in vec4 ParticleColor;
in vec3 fNor;
in vec2 uv;
out vec4 FragColor;

uniform float alpha = 0.5f;
uniform sampler2D particleTexture;

void main()
{
    vec4 diffuse = texture(particleTexture, uv);
            if (alpha < 0.01)
                discard;
        FragColor = vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a * alpha);
}