#version 400 core
out vec4 FragColor;
in vec3 fNor;
in vec2 uv;
uniform sampler2D text;
uniform float alpha;

void main()
{
    vec4 diffuse = texture(text, uv);
        if (alpha < 0.01)
            discard;
    FragColor = vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a * alpha);
}