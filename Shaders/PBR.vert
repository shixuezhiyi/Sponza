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
    mat3 TBN;
} vertOut;

mat3 getTBN()
{
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    if (hasTangent)
    {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 N = normalize(normalMatrix * aNormal);
        vec3 B = normalize(cross(T, N));
        mat3 TBN = transpose(mat3(T, B, N));
        return TBN;
    }
    //TODO 手动计算
    return normalMatrix;
}

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vertOut.texCoord = vec2(aTexCoord.x, 1 - aTexCoord.y);
    vertOut.TBN = getTBN();
    vertOut.normal = aNormal;
    vertOut.fragPos = vec3(model * vec4(aPos, 1.0));
}