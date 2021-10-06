// ���ʃV�F�[�_�[
/*
DX11�ł́A���\�[�X���o�C���h�ł�����ʂ̃X���b�g���^�����܂��B
�ő�15�̒萔�o�b�t�@�[�A16�̃T���v���[�A�ő�128�̃e�N�X�`���A
8�̃����_�[�^�[�Q�b�g/ UAV���o�C���h�ł��܂��B
Vulkan / DX12�ł́A���̌���͊J���҂ɔC����Ă��܂��B
*/

#include "Light.hlsli"

// �{�[����
#define MAX_BONE (512)

// �V�X�e���萔�o�b�t�@
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

// �g�����X�t�H�[���萔�o�b�t�@
cbuffer Transform : register(b6)
{
	float4x4 _mWorld;
}

cbuffer Animation : register(b7)
{
	float4x4 _mBone[MAX_BONE];
}

Texture2D		_MainTexture	: register(t4); // ���C���e�N�X�`��
Texture2D		_ShadowTexture	: register(t5); // �V���h�E�}�b�v
Texture2D		_SkyTexture		: register(t6);	// �X�J�C�{�b�N�X

StructuredBuffer<PointLightData> _PointLights : register(t8);	// �|�C���g���C�g
StructuredBuffer<SpotLightData>  _SpotLights  : register(t9);	// �X�|�b�g���C�g

SamplerState	_MainSampler	: register(s4); // ���C���T���v��
SamplerState	_ShadowSampler	: register(s5); // �V���h�E�}�b�v
SamplerState	_SkySampler		: register(s6); // �X�J�C�{�b�N�X


// �����ɂ͐��K�����ꂽ���˃x�N�g������
float2 SkyMapEquirect(float3 reflectionVector)
{
	float PI = 3.14159265359f;
	float phi = acos(-reflectionVector.y);
	float theta = atan2(-1.0f * reflectionVector.x, reflectionVector.z) + PI;
	return float2(theta / (PI * 2.0f), phi / PI);
}

// �[�x���烏�[���h���W�����߂�
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