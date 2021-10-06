// 2D用ピクセルシェーダ
#include "Common.hlsli"

// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	
	if (_Flg & TEXTURE_FLG)
	{
		Color *= _SkyTexture.Sample(_SkySampler, input.TexCoord);
	}
	
	return Color;
}
