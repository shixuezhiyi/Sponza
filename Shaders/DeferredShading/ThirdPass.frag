#version 330
out vec4 FRAGCOLOR;

in vec2 texCoord;
uniform sampler2D gPosition;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoMetallic;
uniform samplerCube shadowMap;
uniform float farPlane;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
const float PI = 3.14159265359;

float getVisibilityPCF(vec3 fragPos)
{
    vec3 disToLight = fragPos - lightPos;
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
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 microfacet()
{
    vec3 normal = texture(gNormalRoughness, texCoord).xyz;
    float roughness = texture(gNormalRoughness, texCoord).w;
    float metallic = texture(gAlbedoMetallic, texCoord).w;
    vec3 albedo = texture(gAlbedoMetallic, texCoord).xyz;
    vec3 fragPos = texture(gPosition, texCoord).xyz;
    vec3 lightD = normalize(lightPos - fragPos);
    float distance = length(lightPos - fragPos);
    float attenuation = 2.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;
    vec3 viewDir = normalize(cameraPos - fragPos);
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
    vec3 Lo = (kD * albedo / PI + specular) * radiance * cosAlpha * getVisibilityPCF(fragPos);
    vec3 ambient = vec3(0.05) * albedo;//环境光
    vec3 color = Lo + ambient;
    return color;
}

//Fresnel-Schlick


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
    //    vec3 color = microfacet();
    //    color = HDRCorrection(color);
    vec3 albedo = texture(gAlbedoMetallic, texCoord).xyz;
    vec3 normal = texture(gNormalRoughness, texCoord).xyz;
//    albedo = gammaCorrection(albedo);
    FRAGCOLOR = vec4(normal, 1.0);
}