
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor; 

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = aColor;
    gl_Position = projection * view * vec4(aPos.xyz, 1.0);
}