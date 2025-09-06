
layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inUv;        // Unused for now
layout(location = 4) in vec4 inFragPosLightSpace;

layout (binding = 3) uniform sampler2D shadowMap;

layout (location = 0) out vec4 fragColor;

// Returns 1 if in shadow, otherwise 0
float calculateShadows(vec4 frag_light_space)
{
    #define BIAS 0.002f

    vec3 normal = normalize(inNormal);
    vec3 lightDir = normalize(lightPosition.xyz - inWorldPos);
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), BIAS);
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    
    // vec3 projCoords = inFragPosLightSpace.xyz / inFragPosLightSpace.w;         // prespective shadows
    vec3 projCoords = inFragPosLightSpace.xyz;
    projCoords = projCoords * 0.5f + 0.5f;
    float currentDepth = projCoords.z;

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float PCFDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > PCFDepth  ? 1.0f : 0.0f;        
        }    
    }

    shadow /= 9.0f;
    if(projCoords.z > 1.0f)
        shadow = 0.0f;
        
    return shadow;
}

void main() {
    vec2(inUv);

    vec3 lightClr = lightColor.rgb;

    // Procedural UV pattern
    vec2 uv_scaled = inUv * 2.0;
    float i_sum = floor(uv_scaled.x) + floor(uv_scaled.y);
    float checker = mod(i_sum, 2.0);
    float darken_factor = mix(1.0, 0.5, checker);

    vec3 color = inColor.rgb * darken_factor;

    // Ambient
    vec3 ambient = 0.1f * color;

    // Diffuse
    vec3 lightDir = normalize(vec3(lightPosition) - inWorldPos);
    vec3 current_normal_value = normalize(inNormal);
    float diff = max(dot(lightDir, current_normal_value), 0.0f);
    vec3 diffuse = diff * lightClr * color;

    // Specular
    vec3 viewDir = normalize(camPos.xyz - inWorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(current_normal_value, halfwayDir), 0.0f), 128.0f); 
    vec3 specular = vec3(2.0f) * lightClr * spec;

    // Shadows
    float shadow = calculateShadows(inFragPosLightSpace);                      
    vec3 clrOutput = (ambient + (1.0f - shadow) * (diffuse + specular)); 

    // Gamma
    clrOutput = pow(clrOutput, vec3(1.0f / 2.2f));
    
    fragColor = vec4(clrOutput, 1.0f);
}