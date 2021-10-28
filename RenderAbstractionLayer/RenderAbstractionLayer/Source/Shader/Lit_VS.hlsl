// 2D用頂点シェーダ

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION;
	float2	TexCoord	: TEXCOORD0;
	float3	Normal	: TEXCOORD1;
	uint		instID : SV_InstanceID;
};

struct VS_OUTPUT {
	float4	Position		: SV_Position;
	float2 TexCoord		: TEXCOORD0;
	float3 WorldPos		: TEXCOORD1;
	float3 Normal		: TEXCOORD2;
};

// トランスフォーム定数バッファ
cbuffer ViewProj : register(b0)
{
	float4x4 _mView;
	float4x4 _mProj;
}

cbuffer World : register(b8)
{
	float4x4 _mWorld[512];
}


VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	int n = input.instID % 512;
	float4x4 mWVP = mul(_mWorld[n], _mView);
	mWVP = mul(mWVP, _mProj);
	
	output.Position = mul(float4(input.Position, 1.0f), mWVP);
	output.TexCoord = input.TexCoord;
	
	output.WorldPos = mul(float4(input.Position, 1.0f), _mWorld[n]).xyz;
	output.Normal = mul(input.Normal, (float3x3) _mWorld[n]).xyz;
	
	return output;
}
