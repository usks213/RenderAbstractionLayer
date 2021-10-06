// ピクセルシェーダ
#include "Common.hlsli"
#include "PBR.hlsli"


// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3	Normal	: NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos : POSITION;
};

// PBRパラメータ
cbuffer Material : register(b0)
{
	float _metallic;
	float _roughness;
	float2 _Dumey;
}


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	
	if (_Flg & TEXTURE_FLG)
	{
		Color *= _MainTexture.Sample(_MainSampler, input.TexCoord);
	}
	
	if (_Flg & LIGHT_FLG)
	{
		float3 L = normalize(-_directionalLit.direction.xyz); // 光源のベクトル
		//float3 L = normalize(float3(1,1,1)); // 光源へのベクトル
		float3 V = normalize(_viewPos.xyz - input.WorldPos); // 視点へのベクトル
		float3 N = normalize(input.Normal); // 法線ベクトル
		float3 H = normalize(L + V); // ハーフベクトル
		
		float3 tangent = normalize(cross(N, float3(1,1,1)));
		float3 bitangent = normalize(cross(N, tangent));
		tangent = normalize(cross(bitangent, N));
		
		float3 Diff = _directionalLit.color.rgb.rgb * Color.rgb * saturate(dot(L, N)); // 拡散色
		Diff += _directionalLit.ambient.rgb * Color.rgb; // 環境光
		float3 Spec = float3(1, 1, 1) *
			pow(saturate(dot(N, H)), 100.0f); // 鏡面反射色
		
		Color.rgb = Diff + Spec;
	}
	
	return Color;
}
