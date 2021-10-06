// 頂点シェーダ
#include "Common.hlsli"

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION0;
	float3	Normal		: NORMAL0;
	float2	TexCoord	: TEXCOORD0;
	float4	Weight		: BLENDWEIGHT0;
	uint4	Index		: BLENDINDICES0;
};

struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float3	Normal		: NORMAL0;
	float2 TexCoord		: TEXCOORD0;
	float3 WorldPos		: POSITION;
};

// スキニング後の頂点・法線
struct SKIN
{
	float4 Pos;
	float3 Norm;
};

// 頂点をスキニング (ボーンにより移動)
SKIN SkinVert(VS_INPUT input)
{
	SKIN output;
	float4 Pos = float4(input.Position, 1.0f);
	float3 Norm = input.Normal;
	if (input.Weight.x == 0.0f)
	{
		// ボーン無し
		output.Pos = Pos;
		output.Norm = Norm;
	}
	else
	{
		// ボーン0
		uint uBone = input.Index.x;
		float fWeight = input.Weight.x;
		matrix m = _mBone[uBone];
		output.Pos = fWeight * mul(Pos, m);
		output.Norm = fWeight * mul(Norm, (float3x3) m);
		// ボーン1
		uBone = input.Index.y;
		fWeight = input.Weight.y;
		m = _mBone[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3) m);
		// ボーン2
		uBone = input.Index.z;
		fWeight = input.Weight.z;
		m = _mBone[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3) m);
		// ボーン3
		uBone = input.Index.w;
		fWeight = input.Weight.w;
		m = _mBone[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3) m);
	}
	return output;
}

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	float4x4 mWVP = mul(_mWorld, _mView);
	mWVP = mul(mWVP, _mProj);
	//float4 pos = float4(input.Position, 1.0f);
	//float3 nrm = input.Normal;
	SKIN vSkinned = SkinVert(input);
	
	// 座標変換
	output.Position = mul(vSkinned.Pos, mWVP);
	output.Normal = mul(vSkinned.Norm, (float3x3) _mWorld).xyz;
	//output.TexCoord = mul(float4(input.TexCoord, 0.0f, 1.0f), _mTex).xy;
	output.TexCoord = input.TexCoord;
	output.WorldPos = mul(vSkinned.Pos, _mWorld).xyz;
	return output;
}
