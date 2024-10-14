#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

// material parameters
uniform sampler2D albedoMap;
uniform vec2 uvLocation;

void main()
{
    FragColor = texture(albedoMap, TexCoords);
}