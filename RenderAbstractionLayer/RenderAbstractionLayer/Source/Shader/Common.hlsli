// 共通シェーダー
/*
DX11では、リソースをバインドできる一定量のスロットが与えられます。
最大15個の定数バッファー、16個のサンプラー、最大128個のテクスチャ、
8個のレンダーターゲット/ UAVをバインドできます。
Vulkan / DX12では、この決定は開発者に任されています。
*/

#include "Light.hlsli"

// ボーン数
#define MAX_BONE (512)

// システム定数バッファ
cbuffer System : register(b5)
{
	float4x4 _mView;
	float4x4 _mProj;
	float4x4 _mProj2D;
	float4x4 _mViewInv;
	float4x4 _mProjInv;
	float4 _viewPos;
	DirectionalLightData _directionalLit;
	uint _pointLightNum;
	uint _spotLightNum;
	float2 _padding2;
}

// トランスフォーム定数バッファ
cbuffer Transform : register(b6)
{
	float4x4 _mWorld;
}

cbuffer Animation : register(b7)
{
	float4x4 _mBone[MAX_BONE];
}

Texture2D		_MainTexture	: register(t4); // メインテクスチャ
Texture2D		_ShadowTexture	: register(t5); // シャドウマップ
Texture2D		_SkyTexture		: register(t6);	// スカイボックス

StructuredBuffer<PointLightData> _PointLights : register(t8);	// ポイントライト
StructuredBuffer<SpotLightData>  _SpotLights  : register(t9);	// スポットライト

SamplerState	_MainSampler	: register(s4); // メインサンプラ
SamplerState	_ShadowSampler	: register(s5); // シャドウマップ
SamplerState	_SkySampler		: register(s6); // スカイボックス


// 引数には正規化された反射ベクトルを代入
float2 SkyMapEquirect(float3 reflectionVector)
{
	float PI = 3.14159265359f;
	float phi = acos(-reflectionVector.y);
	float theta = atan2(-1.0f * reflectionVector.x, reflectionVector.z) + PI;
	return float2(theta / (PI * 2.0f), phi / PI);
}

// 深度からワールド座標を求める
float3 DepthToWorldPosition(float depth, float2 texCoord, float4x4 viewMatrixInv, float4x4 projMatrixInv)
{
	float z = depth * 2.0 - 1.0;

	float4 clipSpacePosition = float4(texCoord * 2.0 - 1.0, z, 1.0);
	float4 viewSpacePosition = mul(projMatrixInv, clipSpacePosition);

    // Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	float4 worldSpacePosition = mul(viewMatrixInv, viewSpacePosition);

	return worldSpacePosition.xyz;
}