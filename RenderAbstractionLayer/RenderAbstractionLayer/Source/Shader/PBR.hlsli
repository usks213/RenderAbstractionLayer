// PBR

#define _PI (3.14159265358979323846f)

// ----------------------------------------------------------------------------
float3 EnvBRDFApprox(float3 SpecularColor, float Roughness, float NoV)
{
	float4 c0 = float4(-1, -0.0275f, -0.572f, 0.022f);
	float4 c1 = float4(1, 0.0425f, 1.04f, -0.04f);
	float4 r = Roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28f * NoV)) * r.x + r.y;
	float2 AB = float2(-1.04f, 1.04f) * a004 + r.zw;
	return SpecularColor * AB.x + AB.y;
}
// ----------------------------------------------------------------------------
// 引数には正規化された反射ベクトルを代入
float2 SkyMapEquirect2(float3 reflectionVector)
{
	float phi = acos(-reflectionVector.y);
	float theta = atan2(-1.0f * reflectionVector.x, reflectionVector.z) + _PI;
	return float2(theta / (_PI * 2.0f), phi / _PI);
}
// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = _PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (float3(1,1,1) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	float3 invRoughness = float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness);
	return F0 + (max(invRoughness, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float3 Fresnel(float3 F0, float3 V, float3 H)
{
	return fresnelSchlick(max(dot(H, V), 0.0), F0);
}
// ----------------------------------------------------------------------------
float3 Fresnel0(float3 albedo, float metallic)
{
	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	float3 M = float3(metallic, metallic, metallic);
	F0 = lerp(F0, albedo, M);
	return F0;
}
// ----------------------------------------------------------------------------
float3 DiffuseBRDF(float3 albedo, float metallic, float3 N, float3 L, float3 F)
{
	//float3 kS = F;
	//float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	//kD *= 1.0 - metallic;
	float3 kD = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f) + F, metallic);
	
	float NdotL = max(dot(N, L), 0.0);
	float3 deffuse = (kD * albedo / _PI) * NdotL;
	
	return deffuse;
}
// ----------------------------------------------------------------------------
float3 SpeculerBRDF(float roughness, float3 N, float3 V, float3 L, float3 H, float3 F)
{
	//float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
	float NdotL = max(dot(N, L), 0.0);
	
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
           
	float3 numerator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
	float3 specular = numerator / denominator;
	
	return specular * NdotL;
}
// ----------------------------------------------------------------------------
float3 AmbientBRDF(float3 albedo, float metallic, float roughness, float ao,
	float3 N, float3 V, float3 R, float3 F0, 
	float3 ambientLight, Texture2D specularSky, SamplerState skySampler)
{
	// ambient lighting (we now use IBL as the ambient term)
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	
	//float3 kS = F;
	//float3 kD = 1.0 - kS;
	//kD *= 1.0 - metallic;
	float3 kD = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f) + F, metallic);

	// ライトマップ? 
	float3 irradiance = ambientLight; //texture(irradianceMap, N).rgb;
	float3 diffuse = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
	const float MAX_REFLECTION_LOD = 14.0f;
	//float3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	//float3 prefilteredColor = specularSky.SampleLevel(skySampler,
	//SkyMapEquirect2(R), roughness * MAX_REFLECTION_LOD).rgb;
	////float2 brdf = texture(brdfLUT, float2(max(dot(N, V), 0.0), roughness)).rg;
	//float3 specular = prefilteredColor;// * (F * brdf.x + brdf.y);

	float3 ref = EnvBRDFApprox(F0, roughness, dot(N, V)) * (1.0f - roughness);
	float mip = MAX_REFLECTION_LOD * roughness;
	float3 specular = specularSky.SampleLevel(skySampler,
		SkyMapEquirect2(reflect(V, N)), mip).rgb * ref;
	
	float3 ambient = (kD * diffuse + specular + diffuse) * ao;
	
	return ambient;
}