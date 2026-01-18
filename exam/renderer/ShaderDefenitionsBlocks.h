#ifndef SHADERDEFINITIONBLOCK_H_
#define SHADERDEFINITIONBLOCK_H_

#include <string>

inline const std::string BLOCK_VERTEX_SHADER_SRC = R"(
    #version 460 core
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 tcoords;
    layout(location = 3) in vec3 color;
    layout(location = 4) in vec4 instaceModelMat0;
    layout(location = 5) in vec4 instaceModelMat1;
    layout(location = 6) in vec4 instaceModelMat2;
    layout(location = 7) in vec4 instaceModelMat3;
    
    uniform mat4 u_ViewProjection;

    out vec3 vertexColor;
    out vec2 vertexTCoords;    
    out vec3 vertexPos;
    out vec3 vertexNormal;

    void main() {
        mat4 model = mat4(instaceModelMat0, instaceModelMat1, 
                          instaceModelMat2, instaceModelMat3);

        vec4 worldPos = model * vec4(position, 1.0);
        gl_Position = u_ViewProjection * worldPos;
        vertexColor = color;    
        vertexTCoords = tcoords;
        vertexPos = worldPos.xyz;
        vertexNormal = normalize(model * vec4(normal,0)).xyz;
    }
)";

inline const std::string BLOCK_FRAGMENT_SHADER_SRC = R"(
    #version 460 core
    in vec3 vertexColor;
    in vec2 vertexTCoords;
    in vec3 vertexPos;
    in vec3 vertexNormal;
    
    uniform float u_BorderWidth;
    uniform vec3 u_BorderColor;
    uniform bool u_ApplyTextures;

    uniform vec3 u_LightPos;
    uniform vec3 u_LightColorDay;
    uniform vec3 u_LightColorNight;
    uniform float u_AmbientStrength;
    uniform float u_SpecularStrength;
    uniform vec3 u_ViewPosition;
    uniform float u_LightMix;
    uniform float u_IsActive;

    layout(binding=1) uniform samplerCube u_cubeTextureSampler;

    out vec4 FragColor;

    void main() {
        vec3 baseColor;
        vec3 lightColor = mix(u_LightColorDay, u_LightColorNight, u_LightMix);

        vec3 ambient = u_AmbientStrength * lightColor;

        vec3 norm = normalize(vertexNormal);
        vec3 lightDir = normalize(u_LightPos - vertexPos);

        float cosAngle = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = cosAngle * lightColor;

        vec3 viewDir = normalize(u_ViewPosition - vertexPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        // 1024 for higher shininess
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 1024);
        vec3 specular = u_SpecularStrength * spec * lightColor;
            
        float maxX = 1.0 - u_BorderWidth;
        float minX = u_BorderWidth;
        float maxY = maxX;
        float minY = minX;

        float alpha = 1.0;

        if(u_IsActive == 1.0){
            alpha = 0.5;
        }
        
        if(u_ApplyTextures){
            vec3 textureColor = texture(u_cubeTextureSampler, vertexPos).rgb;
            baseColor = mix(textureColor, vertexColor, 0.3);
        }else{
            baseColor = vertexColor;
        }
            
        vec3 finalColor = (ambient + diffuse + specular) * baseColor;
        FragColor = vec4(finalColor, alpha);
    }
)";

#endif