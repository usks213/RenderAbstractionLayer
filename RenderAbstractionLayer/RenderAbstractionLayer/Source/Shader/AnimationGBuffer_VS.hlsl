// ���_�V�F�[�_
#include "Common.hlsli"

// �p�����[�^
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

// �X�L�j���O��̒��_�E�@��
struct SKIN
{
	float4 Pos;
	float3 Norm;
};

// ���_���X�L�j���O (�{�[���ɂ��ړ�)
SKIN SkinVert(VS_INPUT input)
{
	SKIN output;
	float4 Pos = float4(input.Position, 1.0f);
	float3 Norm = input.Normal;
	if (input.Weight.x == 0.0f)
	{
		// �{�[������
		output.Pos = Pos;
		output.Norm = Norm;
	}
	else
	{
		// �{�[��0
		uint uBone = input.Index.x;
		float fWeight = input.Weight.x;
		matrix m = _mBone[uBone];
		output.Pos = fWeight * mul(Pos, m);
		output.Norm = fWeight * mul(Norm, (float3x3) m);
		// �{�[��1
		uBone = input.Index.y;
		fWeight = input.Weight.y;
		m = _mBone[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3) m);
		// �{�[��2
		uBone = input.Index.z;
		fWeight = input.Weight.z;
		m = _mBone[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3) m);
		// �{�[��3
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
	
	// ���W�ϊ�
	output.Position = mul(vSkinned.Pos, mWVP);
	output.Normal = mul(vSkinned.Norm, (float3x3) _mWorld).xyz;
	//output.TexCoord = mul(float4(input.TexCoord, 0.0f, 1.0f), _mTex).xy;
	output.TexCoord = input.TexCoord;
	output.WorldPos = mul(vSkinned.Pos, _mWorld).xyz;
	return output;
}
