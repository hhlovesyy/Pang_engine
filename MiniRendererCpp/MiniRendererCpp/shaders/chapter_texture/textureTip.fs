#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

// material parameters
uniform sampler2D albedoMap;
uniform vec2 uvLocation;

bool isAlmostSame(vec2 a, vec2 b)
{
    return abs(a.x - b.x) < 0.01 && abs(a.y - b.y) < 0.01;
}

void main()
{
    FragColor = texture(albedoMap, TexCoords);
    //如果TexCoords基本和uvLocation一样，那么显示红色
    if(isAlmostSame(TexCoords, uvLocation))
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    if(TexCoords.x > 1.0 || TexCoords.y > 1.0)
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
}