#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 particlePosition;
uniform float particleScale;
uniform float particleRotation;

out vec2 texCoords1;
out vec2 texCoords2;

uniform int textureRows;
uniform int currentTextureIndex;
uniform int nextTextureIndex;

void main()
{
    // Camera right and up vectors
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp    = vec3(view[0][1], view[1][1], view[2][1]);

    // Rotate the quad corners
    float cosTheta = cos(particleRotation);
    float sinTheta = sin(particleRotation);
    vec2 rotatedPos = vec2(
        aPos.x * cosTheta - aPos.y * sinTheta,
        aPos.x * sinTheta + aPos.y * cosTheta
    );

    // Scale and position the quad
    vec3 worldPos = particlePosition +
                    cameraRight * rotatedPos.x * particleScale +
                    cameraUp    * rotatedPos.y * particleScale;

    gl_Position = projection * view * vec4(worldPos, 1.0);

    // Calculate the atlas size
    float atlasSize = 1.0 / float(textureRows);

    // Calculate texture offsets for the current and next textures
    vec2 texOffset1 = vec2( (currentTextureIndex % textureRows) * atlasSize,
                            (currentTextureIndex / textureRows) * atlasSize );

    vec2 texOffset2 = vec2( (nextTextureIndex % textureRows) * atlasSize,
                            (nextTextureIndex / textureRows) * atlasSize );

    // Adjust texture coordinates
    vec2 atlasTexCoords = aTexCoords * atlasSize;

    texCoords1 = texOffset1 + atlasTexCoords;
    texCoords2 = texOffset2 + atlasTexCoords;
}
