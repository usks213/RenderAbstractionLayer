// ピクセルシェーダ
#include "Common.hlsli"
#include "PBR.hlsli"
#include "DeferredCommon.hlsli"


// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3	Normal	: NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos	: POSITION;
};


PS_OUTPUT PS(VS_OUTPUT input)
{
	float4 Color = _Color;
	
	if (_Flg & TEXTURE_FLG)
	{
		Color *= _MainTexture.Sample(_MainSampler, input.TexCoord);
	}
	
	PS_OUTPUT output;
	
	output.Out0 = float4(Color.rgb, _metallic);
	output.Out1 = float4(input.Normal, _roughness);
	output.Out2 = float4(input.WorldPos, 1);
	return output;
}
