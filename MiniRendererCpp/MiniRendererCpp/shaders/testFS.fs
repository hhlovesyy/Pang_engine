#version 330 core
out vec4 FragColor;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos; 
uniform Material material;
uniform Light light;

uniform vec4 showThing;  //0:show ambient 1:show diffuse 2.show specular 3:is half lambert

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    // ambient
    vec3 ambient = light.ambient * material.ambient * texColor.rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float isHalfLambert = showThing.w;
    if(isHalfLambert > 0.0)
    {
        float halfLambert = 0.5 * dot(norm, lightDir) + 0.5;
        diff = halfLambert;
    }

    vec3 diffuse = diff * light.diffuse * texColor.rgb;
    
    // specular, phong, not blinn-phong
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;  
        
    vec3 result = (ambient * showThing.x + diffuse * showThing.y + specular * showThing.z);
    FragColor = vec4(result, 1.0);
} 
