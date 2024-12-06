#version 460 core
in vec2 texCoords1;
in vec2 texCoords2;

uniform sampler2D atlasTexture;
uniform float blendFactor;

out vec4 FragColor;

void main()
{
    // Sample the textures
    vec4 color1 = texture(atlasTexture, texCoords1);
    vec4 color2 = texture(atlasTexture, texCoords2);

    vec4 color = mix(color1, color2, blendFactor);

    if (color.a < 0.01)
        discard;

    // Blend between the two textures
    FragColor = color;
}
