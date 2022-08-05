#version 330
out vec4 FRAGCOLOR;


uniform bool hasNormal;
uniform bool hasBaseColor;
uniform bool hasMetallicRoughness;

uniform sampler2D BaseColorTex;
uniform sampler2D NormalTex;
uniform sampler2D MetallicRoughnessTex;
uniform samplerCube shadowMap;
uniform float farPlane;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightPos;
const float PI = 3.14159265359;
in VertOut
{
    vec3 fragPos;
    vec2 texCoord;
    vec3 normal;
} fragIn;


float getVisibility()
{
    float bias = 0.05;
    vec3 disToLight = fragIn.fragPos - lightPos;
    float shadowDepth = texture(shadowMap, disToLight).r * farPlane;
    float dis = length(disToLight);
    return dis - bias < shadowDepth ? 1.0 : 0.0;
}


float getVisibilityPCF()
{
    vec3 disToLight = fragIn.fragPos - lightPos;
    float shadowDepth = texture(shadowMap, disToLight).r * farPlane;
    float dis = length(disToLight);
    float shadow = 0.0;
    float bias = 0.05;
    float sampleNum = 6.0;
    float offset = 0.1;
    for (float x = -offset; x < offset; x += offset / (sampleNum * 0.5))
    {
        for (float y = -offset; y < offset; y += offset / (sampleNum * 0.5))
        {
            for (float z = -offset; z < offset; z += offset / (sampleNum * 0.5))
            {
                float shadowDepth = texture(shadowMap, disToLight).r * farPlane;
                if (dis - bias < shadowDepth)
                shadow += 1.0;
            }
        }
    }
    return shadow / (sampleNum * sampleNum * sampleNum);
}


//float getVisibilityPCSS()
//{
//
//}


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

vec3 microfacet()
{
    vec3 normal = getNormal();
    float roughness = getRoughness();
    float metallic = getMetallic();
    vec3 albedo = getAlbedo();
    vec3 lightD = normalize(lightPos - fragIn.fragPos);
    float distance = length(lightPos - fragIn.fragPos);
    float attenuation = 2.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;
    vec3 viewDir = normalize(cameraPos - fragIn.fragPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
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
    vec3 Lo = (kD * albedo / PI + specular) * radiance * cosAlpha * getVisibilityPCF();
    vec3 ambient = vec3(0.05) * albedo;//环境光
    vec3 color = Lo + ambient;
    return color;
}

vec3 HDRCorrection(vec3 color)
{
    return color / (color + vec3(1.0));
}

vec3 gammaCorrection(vec3 color)
{
    return pow(color, vec3(1.0 / 2.2));
}

void main()
{
    vec3 color = microfacet();
    //    color = HDRCorrection(color);
    color = gammaCorrection(color);
    FRAGCOLOR = vec4(color, 1.0);
}

