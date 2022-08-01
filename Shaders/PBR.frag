#version 330
out vec4 FRAGCOLOR;


uniform bool hasNormal;
uniform bool hasBaseColor;
uniform bool hasMetallicRoughness;

uniform sampler2D BaseColorTex;
uniform sampler2D NormalTex;
uniform sampler2D MetallicRoughnessTex;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 cameraPos;
const float PI = 3.14159265359;
in VertOut
{
    vec3 fragPos;
    vec2 texCoord;
    vec3 normal;
    mat3 TBN;
} fragIn;

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

//GGX
float normalDistirbution(vec3 halfV, vec3 normal, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDoth = max(dot(halfV, normal), 0.0);
    float nDoth2 = nDoth * nDoth;
    float nom = a2;
    float denom = (nDoth2 * (a2 - 1.0) + 1.0);
    return nom / (PI * denom * denom);
}
//SchlickGGX
float SchlickGGX(float nDotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = nDotV;
    float denom = nDotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2 = SchlickGGX(NdotV, roughness);
    float ggx1 = SchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

//Fresnel-Schlick
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


void main()
{
    vec3 normal = getNormal();
    float roughness = getRoughness();
    float metallic = getMetallic();
    vec3 albedo = getAlbedo();
    vec3 lightD  = -lightDir;
    vec3 viewDir = normalize(cameraPos - fragIn.fragPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 radiance = lightColor;
    //lightDir是光源到物体
    vec3 halfV = normalize(viewDir + lightD);
    float NDF = normalDistirbution(halfV, normal, roughness);
    float G = GeometrySmith(normal, viewDir, lightD, roughness);
    vec3 F = fresnelSchlick(max(dot(halfV, viewDir), 0.0), F0);
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightD), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    float cosAlpha = max(dot(normal, lightD), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * cosAlpha;
    vec3 ambient = vec3(0.03) * albedo;//环境光
    vec3 color = Lo + ambient;

    //HDR
    color = color / (color + vec3(1.0));
    //gamma
    color = pow(color, vec3(1.0 / 2.2));



    FRAGCOLOR = vec4(color, 1.0);
}