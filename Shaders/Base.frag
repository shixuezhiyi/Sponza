#version 330
out vec4 FRAGCOLOR;
uniform bool hasNormal;
uniform bool hasBaseColor;
uniform bool hasMetallicRoughness;

uniform sampler2D BaseColorTex;
uniform sampler2D NormalTex;
uniform sampler2D MetallicRoughnessTex;

in vec2 texCoord;

void main()
{
    //    FRAGCOLOR = vec4(texCoord, 1.0, 1.0);
    FRAGCOLOR = texture(BaseColorTex, texCoord);
}