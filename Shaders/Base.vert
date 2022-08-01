#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;
void main()
{

    //因为图片上下颠倒
    texCoord = vec2(aTexCoord.x, 1 - aTexCoord.y);


    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    //    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}