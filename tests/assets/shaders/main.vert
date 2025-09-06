
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in int aColor;     // unused for now

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outUv;
layout(location = 4) out vec4 outFragPosLightSpace;

void main() {
    int(aColor);

    mat4 model = objects[gl_DrawID].model;
    mat4 inverseNormal = objects[gl_DrawID].inverseNormal;
    int color = objects[gl_DrawID].color;
    
    float r = (float(color & 0xff)) / 255.0f;
    float g = (float((color >> 8) & 0xff)) / 255.0f;
    float b = (float((color >> 16) & 0xff)) / 255.0f;
    float a = (float((color >> 24) & 0xff)) / 255.0f;

    outColor = vec4(r, g, b, 1.0f);

    vec4 worldPos = model * vec4(aPos, 1.0);
    outUv = aUv;
    outNormal = mat3(inverseNormal) * aNormal;
    outFragPosLightSpace = lightSpace * worldPos;
    outWorldPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}