// 2D�p�s�N�Z���V�F�[�_

// �p�����[�^
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

// �}�e���A��
cbuffer Material : register(b0)
{
	float3 _Color;
	float _Time;
}

Texture2D _RT			: register(t0);	// �����_�[�^�[�Q�b�g
SamplerState _Sampler		: register(s0);	// ���C���T���v��

float4 PS(VS_OUTPUT input) : SV_Target0
{
	float3 Color = _Color;
	Color *= _RT.Sample(_Sampler, input.TexCoord).rgb;
	
	// �|�X�g�G�t�F�N�g
	//Color.r *= saturate(sin(_Time));
	
	return float4(Color, 1.0f);
}
