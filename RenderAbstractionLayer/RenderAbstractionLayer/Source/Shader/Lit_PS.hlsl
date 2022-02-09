// ライトピクセルシェーダ

#include "Light.hlsli"
#include "Noise.hlsli"

// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos : POSITION0;
	float3 Normal   : NORMAL0;
	float3x3 TBN	: TEXCOORD2;
};

// マテリアル
cbuffer Material : register(b0)
{
	float4 _Color;
	uint _PointLightNum;
	float3 _ViewPos;
	float4 _LitDir;
}

// テクスチャ
Texture2D _Texture : register(t0);				// メインテクスチャ
Texture2D _NormalTexture : register(t1);		// ノーマルテクスチャ
SamplerState _Sampler : register(s0);			// メインサンプラ

// ポイントライト
StructuredBuffer<PointLightData> _PointLights : register(t8); // ポイントライト


float3 Unity_NormalFromHeight_Tangent(float In, float3x3 TBN, float3 Position)
{
	float3 worldDirivativeX = ddx(Position * 100);
	float3 worldDirivativeY = ddy(Position * 100);
	float3 crossX = cross(TBN[2].xyz, worldDirivativeX);
	float3 crossY = cross(TBN[2].xyz, worldDirivativeY);
	float3 d = abs(dot(crossY, worldDirivativeX));
	float3 inToNormal = ((((In + ddx(In)) - In) * crossY) + (((In + ddy(In)) - In) * crossX)) * sign(d);
	inToNormal.y *= -1.0;
	float3 Out = normalize((d * TBN[2].xyz) - inToNormal);
	Out = mul(Out, TBN);
	return Out;
}

float3 Unity_NormalFromHeight_World(float In, float3x3 TBN, float3 Position)
{
	float3 worldDirivativeX = ddx(Position * 100);
	float3 worldDirivativeY = ddy(Position * 100);
	float3 crossX = cross(TBN[2].xyz, worldDirivativeX);
	float3 crossY = cross(TBN[2].xyz, worldDirivativeY);
	float3 d = abs(dot(crossY, worldDirivativeX));
	float3 inToNormal = ((((In + ddx(In)) - In) * crossY) + (((In + ddy(In)) - In) * crossX)) * sign(d);
	inToNormal.y *= -1.0;
	return normalize((d * TBN[2].xyz) - inToNormal);
}

float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	//Color *= _Texture.Sample(_Sampler, input.TexCoord);
	float3 Result = float3(0, 0, 0);

	// パラメータ
	float3 pos = input.WorldPos;
	float3 texNormal = _NormalTexture.Sample(_Sampler, input.TexCoord).rgb;
	texNormal.z *= -1;
	float3 N = normalize(mul(input.TBN, texNormal));
	
	// Height to Normal
	N = normalize(Unity_NormalFromHeight_World(
	fBm(input.TexCoord * 5) * 200, input.TBN, pos));
	N.z *= -1;
	
	//Result = N;
	
	float3 V = normalize(_ViewPos - pos); // 視点へのベクトル
	float3 R = reflect(-V, N);
	
	// ディレクショナルライト
	{
		DirectionalLightData data;
		data.color = float4(1.0f, 1.0f, 1.0f, 0.0f);
		data.ambient = data.color * 0.0f;
		data.direction = _LitDir;
		
		float3 L = normalize(_LitDir);
		float3 H = normalize(L + V);
		
		float3 dif = Color.rgb * saturate(dot(L, N));
		float3 spe = pow(saturate(dot(N, H)), 100.0f);
		float3 amb = float3(0.2f, 0.2f, 0.2f) * Color.rgb;
		
		Result += (dif + spe + amb) * DirectionalLightColor(data);
	}
	
	// ポイントライトライト計算
	uint i = 0;
	for (; i < _PointLightNum; ++i)
	{
		float3 L = normalize(_PointLights[i].position - pos); // 光源のベクトル
		float3 H = normalize(L + V); // ハーフベクトル
		
		float3 dif = Color.rgb * saturate(dot(L, N));
		float3 spe = pow(saturate(dot(N, H)), 100.0f);
		float3 amb = float3(0.2f, 0.2f, 0.2f) * Color.rgb;
		
		Result += (dif + spe + amb) * PointLightColor(_PointLights[i], pos);
	}
	
	
	return float4(Result.rgb, Color.w);
}
