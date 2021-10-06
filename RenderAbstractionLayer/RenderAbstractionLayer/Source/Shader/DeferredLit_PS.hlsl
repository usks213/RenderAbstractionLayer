// ピクセルシェーダ
#include "Common.hlsli"
#include "PBR.hlsli"
#include "DeferredCommon.hlsli"

// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 rt0 = _RT0.Sample(_MainSampler, input.TexCoord);
	float4 rt1 = _RT1.Sample(_MainSampler, input.TexCoord);
	float depth = _Depth.Sample(_ShadowSampler, input.TexCoord);
	
	//float3 pos = DepthToWorldPosition(depth, input.TexCoord, _mViewInv, _mProjInv);
	float3 pos = _RT2.Sample(_MainSampler, input.TexCoord).xyz;
	float3 normal = rt1.xyz;
	float3 albedo = rt0.rgb;
	float metallic = rt0.w;
	float roughness = rt1.w;
	float ao = 1.0f;
	
	float3 color = albedo;
	
	if (_Flg & LIGHT_FLG)
	{
		float3 N = normalize(normal); // 法線ベクトル
		float3 V = normalize(_viewPos.xyz - pos); // 視点へのベクトル
		float3 R = reflect(-V, N);
		
		float3 F0 = Fresnel0(albedo, metallic);
		float3 Lit = float3(0, 0, 0);
		
		// DirectionalLight
		{
			float3 L = normalize(_directionalLit.direction.xyz); // 光源のベクトル
			float3 H = normalize(L + V); // ハーフベクトル
			float3 F = Fresnel(F0, V, H);
		
			float3 diffuse = DiffuseBRDF(albedo, metallic, N, L, F);
			float3 specular = SpeculerBRDF(roughness, N, V, L, H, F);
			
			Lit += (diffuse + specular) * DirectionalLightColor(_directionalLit);
		}
		// PointLight
		int i = 0;
		for (i = 0; i < _pointLightNum; ++i)
		{
			float3 L = normalize(_PointLights[i].position - pos); // 光源のベクトル
			float3 H = normalize(L + V); // ハーフベクトル
			float3 F = Fresnel(F0, V, H);
		
			float3 diffuse = DiffuseBRDF(albedo, metallic, N, L, F);
			float3 specular = SpeculerBRDF(roughness, N, V, L, H, F);
			
			Lit += (diffuse + specular) * PointLightColor(_PointLights[i], pos);
		}
		// SpotLight
		for (i = 0; i < _spotLightNum; ++i)
		{
			float3 L = normalize(_SpotLights[i].position - pos); // 光源のベクトル
			float3 H = normalize(L + V); // ハーフベクトル
			float3 F = Fresnel(F0, V, H);
		
			float3 diffuse = DiffuseBRDF(albedo, metallic, N, L, F);
			float3 specular = SpeculerBRDF(roughness, N, V, L, H, F);
		
			Lit += (diffuse + specular) * SpotLightColor(_SpotLights[i], pos);
		}
		// AmbientLight
		float3 ambient = AmbientBRDF(albedo, metallic, roughness, ao, N, V, R, F0, 
			_directionalLit.ambient.rgb, _SkyTexture, _SkySampler);
		
		color.rgb = ambient + Lit;
	}
	
	// HDR tonemapping
	//color = color / (color + float3(1.0f, 1.0f, 1.0f));
    // gamma correct
	//color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
	
	return float4(color, 1.0f);
}
