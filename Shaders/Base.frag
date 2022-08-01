#version 330
out vec4 FRAGCOLOR;
uniform bool hasNormal;
uniform bool hasBaseColor;
uniform bool hasMetallicRoughness;
uniform float baseTexIdx;
uniform sampler2D BaseColorTex;
uniform sampler2D NormalTex;
uniform sampler2D MetallicRoughnessTex;

in vec2 texCoord;

void main()
{
//    FRAGCOLOR = vec4(baseTexIdx, baseTexIdx, baseTexIdx, 1.0);
        FRAGCOLOR = texture(BaseColorTex, texCoord);
}