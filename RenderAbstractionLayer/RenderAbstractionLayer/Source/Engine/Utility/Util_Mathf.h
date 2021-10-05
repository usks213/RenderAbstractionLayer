/*****************************************************************//**
 * \file   Util_Mathf.h
 * \brief  算術
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/
#ifndef _UTIL_MATHF_
#define _UTIL_MATHF_

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <iostream>

#define _D3D11
#ifdef _D3D11 || _D3D12

#include <DirectXMath.h>
using namespace DirectX;

struct Vector2 : public XMFLOAT2
{
	Vector2() noexcept : XMFLOAT2(0.f, 0.f) {}
	constexpr explicit Vector2(float ix) noexcept : XMFLOAT2(ix, ix) {}
	constexpr Vector2(float ix, float iy) noexcept : XMFLOAT2(ix, iy) {}
	Vector2(FXMVECTOR V) noexcept { XMStoreFloat2(this, V); }
	Vector2(const XMFLOAT2& V) noexcept { this->x = V.x; this->y = V.y; }

	operator XMVECTOR() const noexcept { return XMLoadFloat2(this); }

	Vector2(const Vector2&) = default;
	Vector2& operator=(const Vector2&) = default;

	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	bool operator == (const Vector2& V) const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		return XMVector2Equal(v1, v2);
	}
	bool operator != (const Vector2& V) const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat2(this);
		XMVECTOR v2 = XMLoadFloat2(&V);
		return XMVector2NotEqual(v1, v2);
	}

	Vector2 operator - () const { return Vector2(-x, -y); }
	Vector2 operator + (const Vector2& v2) const
	{
		return Vector2(x + v2.x, y + v2.y);
	}
	Vector2 operator - (const Vector2& v2) const
	{
		return Vector2(x - v2.x, y - v2.y);
	}
	Vector2 operator * (float s) const
	{
		return Vector2(x * s, y * s);
	}
	Vector2 operator / (float s) const
	{
		return Vector2(x / s, y / s);
	}
	Vector2& operator += (const Vector2& v2)
	{
		x += v2.x;
		y += v2.y;
		return *this;
	}
	Vector2 operator -= (const Vector2& v2)
	{
		x -= v2.x;
		y -= v2.y;
		return *this;
	}
	Vector2 operator *= (float s)
	{
		x *= s;
		y *= s;
		return *this;
	}
	Vector2 operator /= (float s)
	{
		x /= s;
		y /= s;
		return *this;
	}
	// ベクトルの長さ
	inline float Length()
	{
		return sqrtf(this->x * this->x + this->y * this->y);
	}
	// ベクトルの正規化
	inline Vector2 Normalize()
	{
		Vector2 vec = *this;
		float mag = vec.Length();
		if (!mag) return vec;
		vec.x /= mag;
		vec.y /= mag;
		return vec;
	}

	// 内積
	inline static float Dot(Vector2 vec1, Vector2 vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y;
	}
	// 壁ずりベクトル
	inline static Vector2 WallScratchVector(Vector2 front, Vector2 normal)
	{
		return (front - normal * Dot(front, normal));
	}
	// 反射ベクトル
	inline static Vector2 WallReflectVector(Vector2 front, Vector2 normal)
	{
		return (front - normal * Dot(front, normal) * 2.0f);
	}
	// 垂直ベクトル
	inline static Vector2 WallVerticalVector(Vector2 front, Vector2 normal)
	{
		return normal * Dot(front, normal);
	}

	static const Vector2 zero;
	static const Vector2 one;
	static const Vector2 right;
	static const Vector2 up;
};
const Vector2 Vector2::zero = { 0,0 };
const Vector2 Vector2::one = { 1,1 };
const Vector2 Vector2::right = { 1,0 };
const Vector2 Vector2::up = { 0,1 };

struct Vector3 : public XMFLOAT3
{
	Vector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f) {}
	constexpr explicit Vector3(float ix) noexcept : XMFLOAT3(ix, ix, ix) {}
	constexpr Vector3(float ix, float iy, float iz) noexcept : XMFLOAT3(ix, iy, iz) {}
	Vector3(FXMVECTOR V) noexcept { XMStoreFloat3(this, V); }
	Vector3(const XMFLOAT3& V) noexcept { this->x = V.x; this->y = V.y; this->z = V.z; }

	operator XMVECTOR() const noexcept { return XMLoadFloat3(this); }

	Vector3(const Vector3&) = default;
	Vector3& operator=(const Vector3&) = default;

	Vector3(Vector3&&) = default;
	Vector3& operator=(Vector3&&) = default;

	bool operator == (const Vector3& V) const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		return XMVector3Equal(v1, v2);
	}
	bool operator != (const Vector3& V) const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		return XMVector3NotEqual(v1, v2);
	}

	Vector3 operator - () const { return Vector3(-x, -y, -z); }
	Vector3 operator + (const Vector3& v2) const
	{
		return Vector3(x + v2.x, y + v2.y, z + v2.z);
	}
	Vector3 operator - (const Vector3& v2) const
	{
		return Vector3(x - v2.x, y - v2.y, z - v2.z);
	}
	Vector3 operator * (float s) const
	{
		return Vector3(x * s, y * s, z * s);
	}
	Vector3 operator / (float s) const
	{
		return Vector3(x / s, y / s, z / s);
	}
	Vector3& operator += (const Vector3& v2)
	{
		x += v2.x;
		y += v2.y;
		z += v2.z;
		return *this;
	}
	Vector3 operator -= (const Vector3& v2)
	{
		x -= v2.x;
		y -= v2.y;
		z -= v2.z;
		return *this;
	}
	Vector3 operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector3 operator /= (float s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
	// ベクトルの長さ
	inline float Length()
	{
		return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	}
	// ベクトルの正規化
	inline Vector3 Normalize()
	{
		Vector3 vec = *this;
		float mag = vec.Length();
		if (!mag) return vec;
		vec.x /= mag;
		vec.y /= mag;
		vec.z /= mag;
		return vec;
	}

	// 内積
	inline static float Dot(Vector3 vec1, Vector3 vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}
	// 壁ずりベクトル
	inline static Vector3 WallScratchVector(Vector3 front, Vector3 normal)
	{
		return (front - normal * Dot(front, normal));
	}
	// 反射ベクトル
	inline static Vector3 WallReflectVector(Vector3 front, Vector3 normal)
	{
		return (front - normal * Dot(front, normal) * 2.0f);
	}
	// 垂直ベクトル
	inline static Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
	{
		return normal * Dot(front, normal);
	}

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 forward;
};
const Vector3 Vector3::zero = { 0,0,0 };
const Vector3 Vector3::one = { 1,1,1 };
const Vector3 Vector3::right = { 1,0,0 };
const Vector3 Vector3::up = { 0,1,0 };
const Vector3 Vector3::forward = { 0,0,1 };


struct Vector4 : public XMFLOAT4
{
	Vector4() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 0.f) {}
	constexpr explicit Vector4(float ix) noexcept : XMFLOAT4(ix, ix, ix, ix) {}
	constexpr Vector4(float ix, float iy, float iz, float iw) noexcept : XMFLOAT4(ix, iy, iz, iw) {}
	Vector4(FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }
	Vector4(const XMFLOAT4& V) noexcept { this->x = V.x; this->y = V.y; this->z = V.z; this->w = V.w; }

	operator XMVECTOR() const  noexcept { return XMLoadFloat4(this); }

	Vector4(const Vector4&) = default;
	Vector4& operator=(const Vector4&) = default;

	Vector4(Vector4&&) = default;
	Vector4& operator=(Vector4&&) = default;

	bool operator == (const Vector4& V) const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		return XMVector4Equal(v1, v2);
	}
	bool operator != (const Vector4& V) const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		return XMVector4NotEqual(v1, v2);
	}

	Vector4 operator - () const { return Vector4(-x, -y, -z, -w); }
	Vector4 operator + (const Vector4& v2) const
	{
		return Vector4(x + v2.x, y + v2.y, z + v2.z, w + v2.w);
	}
	Vector4 operator - (const Vector4& v2) const
	{
		return Vector4(x - v2.x, y - v2.y, z - v2.z, w - v2.w);
	}
	Vector4 operator * (float s) const
	{
		return Vector4(x * s, y * s, z * s, w * s);
	}
	Vector4 operator / (float s) const
	{
		return Vector4(x / s, y / s, z / s, w / s);
	}
	Vector4& operator += (const Vector4& v2)
	{
		x += v2.x;
		y += v2.y;
		z += v2.z;
		w += v2.w;
		return *this;
	}
	Vector4 operator -= (const Vector4& v2)
	{
		x -= v2.x;
		y -= v2.y;
		z -= v2.z;
		w -= v2.w;
		return *this;
	}
	Vector4 operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	Vector4 operator /= (float s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}
	// ベクトルの長さ
	inline float Length()
	{
		return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
	}
	// ベクトルの正規化
	inline Vector4 Normalize()
	{
		Vector4 vec = *this;
		float mag = vec.Length();
		if (!mag) return vec;
		vec.x /= mag;
		vec.y /= mag;
		vec.z /= mag;
		vec.w /= mag;
		return vec;
	}

};

struct Matrix : public XMFLOAT4X4
{
	Matrix() noexcept
		: XMFLOAT4X4(1.f, 0, 0, 0,
			0, 1.f, 0, 0,
			0, 0, 1.f, 0,
			0, 0, 0, 1.f) {}
	constexpr Matrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) noexcept
		: XMFLOAT4X4(m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33) {}
	explicit Matrix(const Vector3& r0, const Vector3& r1, const Vector3& r2) noexcept
		: XMFLOAT4X4(r0.x, r0.y, r0.z, 0,
			r1.x, r1.y, r1.z, 0,
			r2.x, r2.y, r2.z, 0,
			0, 0, 0, 1.f) {}
	explicit Matrix(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) noexcept
		: XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w,
			r1.x, r1.y, r1.z, r1.w,
			r2.x, r2.y, r2.z, r2.w,
			r3.x, r3.y, r3.z, r3.w) {}
	Matrix(const XMFLOAT4X4& M) noexcept { std::memcpy(this, &M, sizeof(XMFLOAT4X4)); }

	explicit Matrix(_In_reads_(16) const float* pArray) noexcept : XMFLOAT4X4(pArray) {}
	Matrix(CXMMATRIX M) noexcept { XMStoreFloat4x4(this, M); }

	Matrix(const Matrix&) = default;
	Matrix& operator=(const Matrix&) = default;

	Matrix(Matrix&&) = default;
	Matrix& operator=(Matrix&&) = default;

	operator XMMATRIX() const noexcept { return XMLoadFloat4x4(this); }

	Vector3 Up() const noexcept { return Vector3(_21, _22, _23); }
	void Up(const Vector3& v) noexcept { _21 = v.x; _22 = v.y; _23 = v.z; }

	Vector3 Down() const  noexcept { return Vector3(-_21, -_22, -_23); }
	void Down(const Vector3& v) noexcept { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

	Vector3 Right() const noexcept { return Vector3(_11, _12, _13); }
	void Right(const Vector3& v) noexcept { _11 = v.x; _12 = v.y; _13 = v.z; }

	Vector3 Left() const noexcept { return Vector3(-_11, -_12, -_13); }
	void Left(const Vector3& v) noexcept { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

	Vector3 Forward() const noexcept { return Vector3(-_31, -_32, -_33); }
	void Forward(const Vector3& v) noexcept { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

	Vector3 Backward() const noexcept { return Vector3(_31, _32, _33); }
	void Backward(const Vector3& v) noexcept { _31 = v.x; _32 = v.y; _33 = v.z; }

	Vector3 Translation() const  noexcept { return Vector3(_41, _42, _43); }
	void Translation(const Vector3& v) noexcept { _41 = v.x; _42 = v.y; _43 = v.z; }

};

#endif // _D3D

struct VectorUint2
{
	VectorUint2() {
		x = y = 0;
	}

	union
	{
		struct
		{
			int x, y;
		};
		int n[2];
	};
};
struct VectorUint3
{
	VectorUint3() {
		x = y = z = 0;
	}

	union
	{
		struct
		{
			int x, y, z;
		};
		int n[3];
	};
};
struct VectorUint4
{
	VectorUint4() {
		x = y = z = w = 0;
	}

	union 
	{
		struct
		{
			int x, y, z, w;
		};
		int n[4];
	};
};


class AABB {
public:
	Vector3 min;
	Vector3 max;

	AABB() :
		min(-0.5f, -0.5f, -0.5f),
		max(0.5f, 0.5f, 0.5f)
	{
	}

public:
	Vector3 center() const { return (max + min) * 0.5f; };
	Vector3 size() const { return max - min; };

	//static void transformAABB(const Matrix& mWorld, const AABB& sourceAABB, AABB& outAABB) {
	//	Vector3 pos[8] = {
	//		// 上面
	//		{ sourceAABB.max.x, sourceAABB.max.y, sourceAABB.max.z },
	//		{ sourceAABB.min.x, sourceAABB.max.y, sourceAABB.max.z },
	//		{ sourceAABB.max.x, sourceAABB.max.y, sourceAABB.min.z },
	//		{ sourceAABB.min.x, sourceAABB.max.y, sourceAABB.min.z },
	//		// 下面
	//		{ sourceAABB.max.x, sourceAABB.min.y, sourceAABB.max.z },
	//		{ sourceAABB.min.x, sourceAABB.min.y, sourceAABB.max.z },
	//		{ sourceAABB.max.x, sourceAABB.min.y, sourceAABB.min.z },
	//		{ sourceAABB.min.x, sourceAABB.min.y, sourceAABB.min.z },
	//	};

	//	outAABB.max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//	outAABB.min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	//	for (auto& p : pos) {
	//		Vector3 wPos = p.Transform(p, mWorld);
	//		outAABB.max = outAABB.max.Max(outAABB.max, wPos);
	//		outAABB.min = outAABB.min.Min(outAABB.min, wPos);
	//	}
	//}
};

//class Frustum
//{
//public:
//	/// @brief コンストラクタ
//	/// @param worldMatrix ワールドマトリックス
//	/// @param fov FOV
//	/// @param aspect アスペクト比
//	/// @param nearZ ニアクリップ
//	/// @param farZ ファークリップ
//	explicit Frustum(const Matrix& worldMatrix, float fov, float aspect, float nearZ, float farZ)
//	{
//		CreateViewFrustum(fov, aspect, nearZ, farZ);
//		UpdateViewFrustum(worldMatrix);
//	}
//
//	/// @brief コンストラクタ
//	/// @param screenDepth 最大の深さ
//	/// @param viewMatrix ビューマトリックス
//	/// @param projectionMatrix プロジェクションマトリックス
//	explicit Frustum(float screenDepth, Matrix& viewMatrix, Matrix& projectionMatrix)
//	{
//		Matrix projMatrix = projectionMatrix;
//		float zMinimum = -projMatrix._43 / projMatrix._33;
//		float r = screenDepth / (screenDepth - zMinimum);
//		projMatrix._33 = r;
//		projMatrix._43 = -r * zMinimum;
//
//		Matrix matrix = XMMatrixMultiply(viewMatrix, projMatrix);
//		float a, b, c, d;
//
//		// near
//		a = matrix._14 + matrix._13;
//		b = matrix._24 + matrix._23;
//		c = matrix._34 + matrix._33;
//		d = matrix._44 + matrix._43;
//		m_planes[0] = XMVectorSet(a, b, c, d);
//		m_planes[0] = XMPlaneNormalize(m_planes[0]);
//
//		// far
//		a = matrix._14 - matrix._13;
//		b = matrix._24 - matrix._23;
//		c = matrix._34 - matrix._33;
//		d = matrix._44 - matrix._43;
//		m_planes[1] = XMVectorSet(a, b, c, d);
//		m_planes[1] = XMPlaneNormalize(m_planes[1]);
//
//		// left
//		a = matrix._14 + matrix._11;
//		b = matrix._24 + matrix._21;
//		c = matrix._34 + matrix._31;
//		d = matrix._44 + matrix._41;
//		m_planes[2] = XMVectorSet(a, b, c, d);
//		m_planes[2] = XMPlaneNormalize(m_planes[2]);
//
//		// right
//		a = matrix._14 - matrix._11;
//		b = matrix._24 - matrix._21;
//		c = matrix._34 - matrix._31;
//		d = matrix._44 - matrix._41;
//		m_planes[3] = XMVectorSet(a, b, c, d);
//		m_planes[3] = XMPlaneNormalize(m_planes[3]);
//
//		// top
//		a = matrix._14 - matrix._12;
//		b = matrix._24 - matrix._22;
//		c = matrix._34 - matrix._32;
//		d = matrix._44 - matrix._42;
//		m_planes[4] = XMVectorSet(a, b, c, d);
//		m_planes[4] = XMPlaneNormalize(m_planes[4]);
//
//		// bottom
//		a = matrix._14 + matrix._12;
//		b = matrix._24 + matrix._22;
//		c = matrix._34 + matrix._32;
//		d = matrix._44 + matrix._42;
//		m_planes[5] = XMVectorSet(a, b, c, d);
//		m_planes[5] = XMPlaneNormalize(m_planes[5]);
//	}
//
//	~Frustum() = default;
//
//public:
//	bool CheckPoint(float x, float y, float z)
//	{
//		for (int i = 0; i < 6; i++)
//		{
//			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f)));
//			if (ret < 0.0f)
//				return false;
//		}
//
//		return true;
//	}
//
//	bool CheckCube(const Vector3& center, float size)
//	{
//		for (int i = 0; i < 6; i++)
//		{
//			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y - size), (center.z - size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y - size), (center.z - size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y + size), (center.z - size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y + size), (center.z - size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y - size), (center.z + size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y - size), (center.z + size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y + size), (center.z + size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y + size), (center.z + size), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			return false;
//		}
//
//		return true;
//	}
//
//	bool CheckSphere(const Vector3& center, float radius)
//	{
//		for (int i = 0; i < 6; i++)
//		{
//			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(center.x, center.y, center.z, 1.0f)));
//			if (ret < -radius)
//				return false;
//		}
//
//		return true;
//	}
//
//	bool CheckRectangle(const Vector3& center, const Vector3& size)
//	{
//		for (int i = 0; i < 6; i++)
//		{
//			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y - size.y), (center.z - size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y - size.y), (center.z - size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y + size.y), (center.z - size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y - size.y), (center.z + size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y + size.y), (center.z - size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y - size.y), (center.z + size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y + size.y), (center.z + size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y + size.y), (center.z + size.z), 1.0f)));
//			if (ret >= 0.0f)
//				continue;
//
//			return false;
//		}
//
//		return true;
//	}
//
//	bool CheckAABB(const AABB& aabb)
//	{
//		return CheckRectangle(aabb.center(), aabb.size());
//	}
//
//private:
//	// プロジェクション変換パラメータから視錐台生成
//	void CreateViewFrustum(float fov, float aspect, float nearZ, float farZ)
//	{
//		// 0:前 1:後ろ 2:左 3:右 4:上 5:下
//		float fTan =
//			tanf(XMConvertToRadians(fov) * 0.5f);
//		float fTan2 = fTan * aspect;
//		m_planes[0] = XMFLOAT4(0.0f, 0.0f, 1.0f, -nearZ);
//		m_planes[1] = XMFLOAT4(0.0f, 0.0f, -1.0f, farZ);
//		m_planes[2] = XMFLOAT4(1.0f, 0.0f, fTan2, 0.0f);
//		m_planes[3] = XMFLOAT4(-1.0f, 0.0f, fTan2, 0.0f);
//		m_planes[4] = XMFLOAT4(0.0f, -1.0f, fTan, 0.0f);
//		m_planes[5] = XMFLOAT4(0.0f, 1.0f, fTan, 0.0f);
//		// 0～3を正規化
//		for (int i = 0; i < 6; ++i) {
//			m_planes[i].Normalize();
//		}
//	}
//
//	// カメラのワールドマトリックスから視錐台を移動
//	void UpdateViewFrustum(const Matrix& worldMatrix)
//	{
//		// CalcWorldMatrixでm_mtxWorldが更新済を前提
//		XMMATRIX mW = XMLoadFloat4x4(&worldMatrix);
//		mW = XMMatrixInverse(nullptr, mW);//逆行列
//		mW = XMMatrixTranspose(mW);//転置行列
//		// ※逆行列の転置行列を求めるのは
//		//   XMPlaneTransformの仕様!!
//		for (int i = 0; i < 6; ++i) {
//			m_planes[i] = XMPlaneTransform(m_planes[i], mW);
//		}
//	}
//
//private:
//	// 0:前 1:後ろ 2:左 3:右 4:上 5:下
//	Plane m_planes[6];
//};


namespace Mathf
{
	// 内積
	inline float Dot(Vector3 vec1, Vector3 vec2)
	{
		float dot;

		dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;

		return dot;
	}

	// 外積
	inline Vector3 Cross(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec;
		vec.x = vec1.y * vec2.z - vec1.z * vec2.y;
		vec.y = vec1.z * vec2.x - vec1.x * vec2.z;
		vec.z = vec1.x * vec2.y - vec1.y * vec2.x;
		return vec;
	}

	// 三点外積
	inline Vector3 Cross(Vector3 center, Vector3 pos1, Vector3 pos2)
	{
		return Cross(pos1 - center, pos2 - center);
	}

	// 外積2D
	inline float Cross2D(Vector3 vec1, Vector3 vec2)
	{
		return vec1.x * vec2.y - vec1.y * vec2.x;
	}

	// ベクトルの長さ
	inline float Length(Vector3 vec)
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// 二点間の距離
	inline float Length(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec = vec1 - vec2;
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// ベクトルの正規化
	inline Vector3 Normalize(Vector3 v)
	{
		Vector3 vec = v;
		float mag = Length(vec);
		if (!mag) return vec;
		vec.x /= mag;
		vec.y /= mag;
		vec.z /= mag;

		return vec;
	}

	// 壁ずりベクトル
	inline Vector3 WallScratchVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return (front - normal * Dot(front, normal));
	}

	// 反射ベクトル
	inline Vector3 WallReflectVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return (front - normal * Dot(front, normal) * 2.0f);
	}

	// 垂直ベクトル
	inline Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return normal * Dot(front, normal);
	}

	// X軸回転
	inline Vector3 RotationX(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.y = vec.y * cosf(rad) + vec.z * sinf(rad);
		v.z = -vec.y * sinf(rad) + vec.z * cosf(rad);

		return v;
	}

	// Y軸回転
	inline Vector3 RotationY(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.x = vec.x * cosf(rad) - vec.z * sinf(rad);
		v.z = vec.x * sinf(rad) + vec.z * cosf(rad);

		return v;
	}

	// Z軸回転
	inline Vector3 RotationZ(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.x = vec.x * cosf(rad) + vec.y * sinf(rad);
		v.y = -vec.x * sinf(rad) + vec.y * cosf(rad);

		return v;
	}

	inline float lerpf(float a, float b, float t) noexcept
	{
		return a + t * (b - a);
	}

	/*inline Vector2 random2(Vector2 st) {
		st = Vector2(st.Dot(Vector2(127.1f, 311.7f)),
			st.Dot(Vector2(269.5f, 183.3f)));
		Vector2 result;
		float temp = 0;
		result.x = -1.0f + 2.0f * modf(sinf(st.x) * 43758.5453123f, &temp);
		result.y = -1.0f + 2.0f * modf(sinf(st.y) * 43758.5453123f, &temp);
		return result;
	}

	inline float perlinNoise(Vector2 st)
	{
		Vector2 p = Vector2(floorf(st.x), floorf(st.y));
		float temp = 0;
		Vector2 f = Vector2(modf(st.x, &temp), modf(st.y, &temp));
		Vector2 u = f * f * (Vector2(1.0f, 1.0f) * 3.0f - 2.0f * f);

		Vector2 v00 = random2(p + Vector2(0, 0));
		Vector2 v10 = random2(p + Vector2(1, 0));
		Vector2 v01 = random2(p + Vector2(0, 1));
		Vector2 v11 = random2(p + Vector2(1, 1));

		return lerpf(
			lerpf(v00.Dot(f - Vector2(0, 0)), v10.Dot(f - Vector2(1, 0)), u.x),
			lerpf(v01.Dot(f - Vector2(0, 1)), v11.Dot(f - Vector2(1, 1)), u.x),
			u.y) + 0.5f;
	}

	inline float fBm(Vector2 st)
	{
		float f = 0;
		Vector2 q = st;

		f += 0.5000f * perlinNoise(q); q = q * 2.01f;
		f += 0.2500f * perlinNoise(q); q = q * 2.02f;
		f += 0.1250f * perlinNoise(q); q = q * 2.03f;
		f += 0.0625f * perlinNoise(q); q = q * 2.01f;

		return f;
	}*/
}

#endif // !_UTIL_MATHF_
