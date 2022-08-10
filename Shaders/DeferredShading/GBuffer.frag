#version 330
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec4 gNormalRoughness;
layout (location = 2) out vec4 gAlbedoMetallic;

uniform bool hasNormal;
uniform bool hasBaseColor;
uniform bool hasMetallicRoughness;

uniform sampler2D BaseColorTex;
uniform sampler2D NormalTex;
uniform sampler2D MetallicRoughnessTex;
uniform vec2 nearAndFar;
in VertOut
{
    vec3 fragPos;
    vec2 texCoord;
    vec3 normal;
} fragIn;

float LinearizeDepth(float depth)
{
    float near = nearAndFar.x;
    float far = nearAndFar.y;
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float getMetallic()
{
    if (hasMetallicRoughness)
    {
        return texture(MetallicRoughnessTex, fragIn.texCoord).b;
    }
    else
    {
        return 1.0f;
    }
}
// gamma校正
vec3 getAlbedo()
{
    return pow(texture(BaseColorTex, fragIn.texCoord).rgb, vec3(2.2));
}
float getRoughness()
{
    if (hasMetallicRoughness)
    {
        return texture(MetallicRoughnessTex, fragIn.texCoord).g;
    }
    else
    {
        return 1.0f;
    }
}

vec3 getNormal()
{
    vec3 Q1 = dFdx(fragIn.fragPos);
    vec3 Q2 = dFdy(fragIn.fragPos);
    vec2 st1 = dFdx(fragIn.texCoord);
    vec2 st2 = dFdy(fragIn.texCoord);

    vec3 N = normalize(fragIn.normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = - normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    vec3 n = fragIn.normal;
    if (hasNormal)
    {
        n = texture(NormalTex, fragIn.texCoord).rgb;
        n = n * 2.0 - 1.0;
        n = normalize(n);
        return normalize(TBN * n);
    }
    return n;
}
void main()
{
    gPositionDepth.rgb = fragIn.fragPos;
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gNormalRoughness.rgb = getNormal();
    gNormalRoughness.a = getRoughness();
    gAlbedoMetallic.rgb = getAlbedo();
    gAlbedoMetallic.a = getMetallic();
}