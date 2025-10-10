
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in int aColor;

void main()
{
    mat4 model = objects[gl_DrawID].model;

    gl_Position = lightSpace * model * vec4(aPos, 1.0);
}