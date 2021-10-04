///*****************************************************************//**
// * \file   Util_Mathf.h
// * \brief  算術
// * 
// * \author USAMI KOSHI
// * \date   2021/10/01
// *********************************************************************/
//#pragma once
//
//
//#include <d3d11_1.h>
//#include <DirectXMath.h>
//using namespace DirectX;
//
//
//#ifdef min
//#undef min
//#endif
//#ifdef max
//#undef max
//#endif
//
//
//struct VectorUint4
//{
//	VectorUint4() {
//		x = y = z = w = 0;
//	}
//
//	union 
//	{
//		struct
//		{
//			int x, y, z, w;
//		};
//		int n[4];
//	};
//};
//
//
//
//namespace Mathf
//{
//	// 内積
//	inline float Dot(Vector3 vec1, Vector3 vec2)
//	{
//		float dot;
//
//		dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
//
//		return dot;
//	}
//
//	// 外積
//	inline Vector3 Cross(Vector3 vec1, Vector3 vec2)
//	{
//		Vector3 vec;
//		vec.x = vec1.y * vec2.z - vec1.z * vec2.y;
//		vec.y = vec1.z * vec2.x - vec1.x * vec2.z;
//		vec.z = vec1.x * vec2.y - vec1.y * vec2.x;
//		return vec;
//	}
//
//	// 三点外積
//	inline Vector3 Cross(Vector3 center, Vector3 pos1, Vector3 pos2)
//	{
//		return Cross(pos1 - center, pos2 - center);
//	}
//
//	// 外積2D
//	inline float Cross2D(Vector3 vec1, Vector3 vec2)
//	{
//		return vec1.x * vec2.y - vec1.y * vec2.x;
//	}
//
//	// ベクトルの長さ
//	inline float Length(Vector3 vec)
//	{
//		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
//	}
//
//	// 二点間の距離
//	inline float Length(Vector3 vec1, Vector3 vec2)
//	{
//		Vector3 vec = vec1 - vec2;
//		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
//	}
//
//	// ベクトルの正規化
//	inline Vector3 Normalize(Vector3 v)
//	{
//		Vector3 vec = v;
//		float mag = Length(vec);
//		if (!mag) return vec;
//		vec.x /= mag;
//		vec.y /= mag;
//		vec.z /= mag;
//
//		return vec;
//	}
//
//	// 壁ずりベクトル
//	inline Vector3 WallScratchVector(Vector3 front, Vector3 normal)
//	{
//		normal.Normalize();
//		return (front - normal * Dot(front, normal));
//	}
//
//	// 反射ベクトル
//	inline Vector3 WallReflectVector(Vector3 front, Vector3 normal)
//	{
//		normal.Normalize();
//		return (front - normal * Dot(front, normal) * 2.0f);
//	}
//
//	// 垂直ベクトル
//	inline Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
//	{
//		normal.Normalize();
//		return normal * Dot(front, normal);
//	}
//
//	// X軸回転
//	inline Vector3 RotationX(const Vector3& vec, float angle)
//	{
//		float rad = DirectX::XMConvertToRadians(angle);
//		Vector3 v = vec;
//
//		v.y = vec.y * cosf(rad) + vec.z * sinf(rad);
//		v.z = -vec.y * sinf(rad) + vec.z * cosf(rad);
//
//		return v;
//	}
//
//	// Y軸回転
//	inline Vector3 RotationY(const Vector3& vec, float angle)
//	{
//		float rad = DirectX::XMConvertToRadians(angle);
//		Vector3 v = vec;
//
//		v.x = vec.x * cosf(rad) - vec.z * sinf(rad);
//		v.z = vec.x * sinf(rad) + vec.z * cosf(rad);
//
//		return v;
//	}
//
//	// Z軸回転
//	inline Vector3 RotationZ(const Vector3& vec, float angle)
//	{
//		float rad = DirectX::XMConvertToRadians(angle);
//		Vector3 v = vec;
//
//		v.x = vec.x * cosf(rad) + vec.y * sinf(rad);
//		v.y = -vec.x * sinf(rad) + vec.y * cosf(rad);
//
//		return v;
//	}
//
//	inline float lerpf(float a, float b, float t) noexcept
//	{
//		return a + t * (b - a);
//	}
//
//	inline Vector2 random2(Vector2 st) {
//		st = Vector2(st.Dot(Vector2(127.1f, 311.7f)),
//			st.Dot(Vector2(269.5f, 183.3f)));
//		Vector2 result;
//		float temp = 0;
//		result.x = -1.0f + 2.0f * modf(sinf(st.x) * 43758.5453123f, &temp);
//		result.y = -1.0f + 2.0f * modf(sinf(st.y) * 43758.5453123f, &temp);
//		return result;
//	}
//
//	inline float perlinNoise(Vector2 st)
//	{
//		Vector2 p = Vector2(floorf(st.x), floorf(st.y));
//		float temp = 0;
//		Vector2 f = Vector2(modf(st.x, &temp), modf(st.y, &temp));
//		Vector2 u = f * f * (Vector2(1.0f, 1.0f) * 3.0f - 2.0f * f);
//
//		Vector2 v00 = random2(p + Vector2(0, 0));
//		Vector2 v10 = random2(p + Vector2(1, 0));
//		Vector2 v01 = random2(p + Vector2(0, 1));
//		Vector2 v11 = random2(p + Vector2(1, 1));
//
//		return lerpf(
//			lerpf(v00.Dot(f - Vector2(0, 0)), v10.Dot(f - Vector2(1, 0)), u.x),
//			lerpf(v01.Dot(f - Vector2(0, 1)), v11.Dot(f - Vector2(1, 1)), u.x),
//			u.y) + 0.5f;
//	}
//
//	inline float fBm(Vector2 st)
//	{
//		float f = 0;
//		Vector2 q = st;
//
//		f += 0.5000f * perlinNoise(q); q = q * 2.01f;
//		f += 0.2500f * perlinNoise(q); q = q * 2.02f;
//		f += 0.1250f * perlinNoise(q); q = q * 2.03f;
//		f += 0.0625f * perlinNoise(q); q = q * 2.01f;
//
//		return f;
//	}
//}