#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float shininess;

uniform vec4 showThing;  //0:show ambient 1:show diffuse 2.show specular 3:is half lambert

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * texColor.rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float isHalfLambert = showThing.w;
    if(isHalfLambert > 0.0)
    {
        float halfLambert = 0.5 * dot(norm, lightDir) + 0.5;
        diff = halfLambert;
    }

    vec3 diffuse = diff * lightColor * texColor.rgb;
    
    // specular, phong, not blinn-phong
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient * showThing.x + diffuse * showThing.y + specular * showThing.z) * objectColor;
    FragColor = vec4(result, 1.0);
} 
