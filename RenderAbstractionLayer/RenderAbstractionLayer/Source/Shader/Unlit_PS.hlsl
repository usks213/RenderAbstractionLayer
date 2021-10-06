// 2D�p�s�N�Z���V�F�[�_
#include "Common.hlsli"

// �p�����[�^
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

// �}�e���A��
cbuffer Material : register(b0)
{
	float4 _Color;
}

float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	Color *= _MainTexture.Sample(_MainSampler, input.TexCoord);
	
	return Color;
}
