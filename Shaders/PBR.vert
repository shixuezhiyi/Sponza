#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool hasTangent;
out vec2 texCoord;
out VertOut
{
    vec3 fragPos;
    vec2 texCoord;
    vec3 normal;
} vertOut;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vertOut.texCoord = vec2(aTexCoord.x, 1 - aTexCoord.y);
    vertOut.normal = aNormal;
    vertOut.fragPos = vec3(model * vec4(aPos, 1.0));
}