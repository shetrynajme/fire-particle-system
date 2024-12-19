#version 460 core
in vec2 texCoords1;
in vec2 texCoords2;

uniform sampler2D atlasTexture;
uniform float blendFactor;
uniform float lifeFactor;

out vec4 FragColor;

void main()
{
    vec4 color1 = texture(atlasTexture, texCoords1);
    vec4 color2 = texture(atlasTexture, texCoords2);

    vec4 baseColor = mix(color1, color2, blendFactor);

    if (baseColor.a < 0.01)
        discard;

    if (lifeFactor < 0.7)
    {
        float t = lifeFactor / 0.7;
        vec3 fireColor = mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 0.5, 0.0), t * 2.0); 
        baseColor.rgb *= fireColor;
    }

    // Output the final color
    FragColor = baseColor;
}
