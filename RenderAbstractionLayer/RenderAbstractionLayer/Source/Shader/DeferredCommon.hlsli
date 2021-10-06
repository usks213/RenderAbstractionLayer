// GBufferシェーダー

// Gbuffer
cbuffer Gbuffer : register(b0)
{
	float _metallic = 1.0f;
	float _roughness = 1.0f;
	float2 _Dumey;
}

// MRT
struct PS_OUTPUT
{
	float4 Out0 : SV_Target0;
	float4 Out1 : SV_Target1;
	float4 Out2 : SV_Target2;
};

Texture2D _RT0 : register(t0); // カラー + メタリック
Texture2D _RT1 : register(t1); // 法線 + ラフネス
Texture2D _RT2 : register(t2); // 座標
Texture2D<float> _Depth : register(t3);	// 深度