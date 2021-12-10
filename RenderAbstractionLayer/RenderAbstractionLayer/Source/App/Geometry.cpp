/*****************************************************************//**
 * \file   Geometry.cpp
 * \brief  �W�I���g��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/03
 *********************************************************************/

#include "Geometry.h"

struct VERTEX_3D 
{
	Vector3 vtx;		// ���_���W
	Vector3 nor;		// �@���x�N�g��
	Vector4 diffuse;	// �g�U���ˌ�
	Vector2 tex;		// �e�N�X�`�����W
};

void Geometry::Quad(core::CoreMesh& out)
{
	const float	SIZE_X = (0.5f); // �����̂̃T�C�Y(X����)
	const float	SIZE_Y = (0.5f); // �����̂̃T�C�Y(Y����)
	const float	SIZE_Z = (0.5f); // �����̂̃T�C�Y(Z����)

	const std::uint32_t QUAD_VERTEX = (4);

	// �v���~�e�B�u�ݒ�
	out.m_topology = core::PrimitiveTopology::TRIANGLE_STRIP;

	VERTEX_3D	vertexWk[QUAD_VERTEX];	// ���_���i�[���[�N

	// ���_���W�̐ݒ�
	// �O
	vertexWk[0].vtx = Vector3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[1].vtx = Vector3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[2].vtx = Vector3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[3].vtx = Vector3(SIZE_X, -SIZE_Y, -SIZE_Z);
	
	// �@���x�N�g���̐ݒ�
	// �O
	vertexWk[0].nor = Vector3(0.0f, 0.0f, 1.0f);
	vertexWk[1].nor = Vector3(0.0f, 0.0f, 1.0f);
	vertexWk[2].nor = Vector3(0.0f, 0.0f, 1.0f);
	vertexWk[3].nor = Vector3(0.0f, 0.0f, 1.0f);

	// �g�U���ˌ��̐ݒ�
	for (std::uint32_t i = 0; i < QUAD_VERTEX; i++)
	{
		vertexWk[i].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (std::uint32_t i = 0; i < QUAD_VERTEX; i += 4)
	{
		vertexWk[0 + i].tex = Vector2(0.0f, 0.0f);
		vertexWk[1 + i].tex = Vector2(1.0f, 0.0f);
		vertexWk[2 + i].tex = Vector2(0.0f, 1.0f);
		vertexWk[3 + i].tex = Vector2(1.0f, 1.0f);
	}

	// ���_����
	out.m_vertexCount = QUAD_VERTEX;
	for (std::uint32_t i = 0; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(vertexWk[i].vtx);
		out.m_vertexData.normals.push_back(vertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(vertexWk[i].tex);
		out.m_vertexData.colors.push_back(vertexWk[i].diffuse);
	}

	// �C���f�b�N�X
	out.m_indexCount = 0;

	// �ڐ��x�N�g���̐���
	CalcTangent(out);
}

void Geometry::Plane(core::CoreMesh& out, int split, float size, float texSize)
{
	int nNumBlockX = split;
	int nNumBlockZ = split;
	float fSizeBlockX = -size;
	float fSizeBlockZ = size;
	float fTexSizeX = texSize;
	float fTexSizeZ = texSize;

	// �v���~�e�B�u��ʐݒ�
	out.m_topology = core::PrimitiveTopology::TRIANGLE_STRIP;
	// ���_���̐ݒ�
	out.m_vertexCount = (nNumBlockX + 1) * (nNumBlockZ + 1);
	// �C���f�b�N�X���̐ݒ�(�k�ރ|���S���p���l������)
	out.m_indexCount = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;
	// ���_�z��̍쐬
	VERTEX_3D* pVertexWk = new VERTEX_3D[out.m_vertexCount];
	// �C���f�b�N�X�z��̍쐬
	int* pIndexWk = new int[out.m_indexCount];
	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;


	for (int z = 0; z < nNumBlockZ + 1; ++z) {
		for (int x = 0; x < nNumBlockX + 1; ++x) {

			// ���_���W�̐ݒ�
			pVtx->vtx.x = x * fSizeBlockX - (nNumBlockX * 0.5f) * fSizeBlockX;
			pVtx->vtx.y = 0.0f;
			pVtx->vtx.z = -z * fSizeBlockZ + (nNumBlockZ * 0.5f) * fSizeBlockZ;
			// �@���̐ݒ�
			pVtx->nor = Vector3(0.0f, 1.0f, 0.0f);
			// ���ˌ��̐ݒ�
			pVtx->diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			// �e�N�X�`�����W�̐ݒ�
			pVtx->tex.x = fTexSizeX * x;
			pVtx->tex.y = fTexSizeZ * z;
			++pVtx;
		}
	}
	//�C���f�b�N�X�z��̒��g�𖄂߂�
	int* pIdx = pIndexWk;
	for (int z = 0; z < nNumBlockZ; ++z) {
		if (z > 0) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (int x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockZ - 1) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	// ���_����
	for (auto i = 0u; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(pVertexWk[i].vtx);
		out.m_vertexData.normals.push_back(pVertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(pVertexWk[i].tex);
		out.m_vertexData.colors.push_back(pVertexWk[i].diffuse);
	}

	// �C���f�b�N�X
	for (auto i = 0; i < out.m_indexCount; ++i)
	{
		out.m_indexData.push_back(pIndexWk[i]);
	}

	// �ڐ��x�N�g���̐���
	CalcTangent(out);

	// �ꎞ�z��̉��
	delete[] pVertexWk;
	delete[] pIndexWk;
}

void Geometry::Cube(core::CoreMesh& out)
{
	const float	SIZE_X = (0.5f); // �����̂̃T�C�Y(X����)
	const float	SIZE_Y = (0.5f); // �����̂̃T�C�Y(Y����)
	const float	SIZE_Z = (0.5f); // �����̂̃T�C�Y(Z����)

	const std::uint32_t CUBE_VERTEX = (24);
	const std::uint32_t CUBE_INDEX = (36);

	// �v���~�e�B�u�ݒ�
	out.m_topology = core::PrimitiveTopology::TRIANGLE_LIST;

	VERTEX_3D	vertexWk[CUBE_VERTEX];	// ���_���i�[���[�N
	std::uint32_t			indexWk[CUBE_INDEX];	// �C���f�b�N�X�i�[���[�N

	// ���_���W�̐ݒ�
	// �O
	vertexWk[0].vtx = Vector3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[1].vtx = Vector3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[2].vtx = Vector3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[3].vtx = Vector3(SIZE_X, -SIZE_Y, -SIZE_Z);
	// �E
	vertexWk[4].vtx = Vector3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[5].vtx = Vector3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[6].vtx = Vector3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[7].vtx = Vector3(SIZE_X, -SIZE_Y, SIZE_Z);
	// ��
	vertexWk[8].vtx = Vector3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[9].vtx = Vector3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[10].vtx = Vector3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[11].vtx = Vector3(SIZE_X, SIZE_Y, -SIZE_Z);
	// ��
	vertexWk[12].vtx = Vector3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[13].vtx = Vector3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[14].vtx = Vector3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[15].vtx = Vector3(-SIZE_X, -SIZE_Y, SIZE_Z);
	// ��
	vertexWk[16].vtx = Vector3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[17].vtx = Vector3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[18].vtx = Vector3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[19].vtx = Vector3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	// ��
	vertexWk[20].vtx = Vector3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[21].vtx = Vector3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[22].vtx = Vector3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[23].vtx = Vector3(-SIZE_X, -SIZE_Y, -SIZE_Z);


	// �@���x�N�g���̐ݒ�
	// �O
	vertexWk[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	vertexWk[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	vertexWk[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	vertexWk[3].nor = Vector3(0.0f, 0.0f, -1.0f);
	// �E
	vertexWk[4].nor = Vector3(1.0f, 0.0f, 0.0f);
	vertexWk[5].nor = Vector3(1.0f, 0.0f, 0.0f);
	vertexWk[6].nor = Vector3(1.0f, 0.0f, 0.0f);
	vertexWk[7].nor = Vector3(1.0f, 0.0f, 0.0f);
	// ��
	vertexWk[8].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertexWk[9].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertexWk[10].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertexWk[11].nor = Vector3(0.0f, 1.0f, 0.0f);
	// ��
	vertexWk[12].nor = Vector3(0.0f, 0.0f, 1.0f);
	vertexWk[13].nor = Vector3(0.0f, 0.0f, 1.0f);
	vertexWk[14].nor = Vector3(0.0f, 0.0f, 1.0f);
	vertexWk[15].nor = Vector3(0.0f, 0.0f, 1.0f);
	// ��
	vertexWk[16].nor = Vector3(-1.0f, 0.0f, 0.0f);
	vertexWk[17].nor = Vector3(-1.0f, 0.0f, 0.0f);
	vertexWk[18].nor = Vector3(-1.0f, 0.0f, 0.0f);
	vertexWk[19].nor = Vector3(-1.0f, 0.0f, 0.0f);
	// ��
	vertexWk[20].nor = Vector3(0.0f, -1.0f, 0.0f);
	vertexWk[21].nor = Vector3(0.0f, -1.0f, 0.0f);
	vertexWk[22].nor = Vector3(0.0f, -1.0f, 0.0f);
	vertexWk[23].nor = Vector3(0.0f, -1.0f, 0.0f);


	// �g�U���ˌ��̐ݒ�
	for (std::uint32_t i = 0; i < CUBE_VERTEX; i++)
	{
		vertexWk[i].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (std::uint32_t i = 0; i < CUBE_VERTEX; i += 4)
	{
		vertexWk[0 + i].tex = Vector2(0.0f, 0.0f);
		vertexWk[1 + i].tex = Vector2(1.0f, 0.0f);
		vertexWk[2 + i].tex = Vector2(0.0f, 1.0f);
		vertexWk[3 + i].tex = Vector2(1.0f, 1.0f);
	}

	// �C���f�b�N�X�z��̐ݒ�
	for (std::uint32_t i = 0, j = 0; i < CUBE_INDEX; i += 6, j += 4)
	{
		indexWk[0 + i] = 0 + j;
		indexWk[1 + i] = 1 + j;
		indexWk[2 + i] = 2 + j;
		indexWk[3 + i] = 2 + j;
		indexWk[4 + i] = 1 + j;
		indexWk[5 + i] = 3 + j;
	}

	// ���_����
	out.m_vertexCount = CUBE_VERTEX;
	for (std::uint32_t i = 0; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(vertexWk[i].vtx);
		out.m_vertexData.normals.push_back(vertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(vertexWk[i].tex);
		out.m_vertexData.colors.push_back(vertexWk[i].diffuse);
	}

	// �C���f�b�N�X
	out.m_indexCount = CUBE_INDEX;
	for (std::uint32_t i = 0; i < out.m_indexCount; ++i)
	{
		out.m_indexData.push_back(static_cast<std::uint32_t>(indexWk[i]));
	}

	// �ڐ��x�N�g���̐���
	CalcTangent(out);
}

void Geometry::Sphere(core::CoreMesh& out, int nSplit, float fSize, float fTexSize)
{
	float nNumBlockX = nSplit;
	float nNumBlockY = nSplit;

	// �v���~�e�B�u��ʐݒ�
	out.m_topology = core::PrimitiveTopology::TRIANGLE_STRIP;
	// ���_���̐ݒ�
	std::uint32_t nNumVertex = (nNumBlockX + 1) * (nNumBlockY + 1);
	// �C���f�b�N�X���̐ݒ�(�k�ރ|���S���p���l������)
	std::uint32_t nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockY + (nNumBlockY - 1) * 2;
	// ���_�z��̍쐬
	VERTEX_3D* pVertexWk = new VERTEX_3D[nNumVertex];
	// �C���f�b�N�X�z��̍쐬
	std::uint32_t* pIndexWk = new std::uint32_t[nNumIndex];
	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;

	for (std::uint32_t y = 0; y < nNumBlockY + 1; ++y) {
		for (std::uint32_t x = 0; x < nNumBlockX + 1; ++x) {

			// ���_���W�̐ݒ�
			pVtx->vtx.x = 0.0f;
			pVtx->vtx.y = 1.0f;
			pVtx->vtx.z = 0.0f;
			// �p�x�ɑ΂����]�}�g���b�N�X�����߂�
			Matrix mR = Matrix::CreateRotationX(Mathf::ToRadians(x * (180.0f / nNumBlockX)));
			mR *= Matrix::CreateRotationY(Mathf::ToRadians(y * -(360.0f / nNumBlockY)));
			// ���W����]�}�g���b�N�X�ŉ�]������
			Vector3 v = pVtx->vtx;
			v = Vector3::Transform(v, mR);
			v = v.Normalize();
			pVtx->vtx = v;

			// �@���̐ݒ�
			pVtx->nor = pVtx->vtx;

			// �傫��
			pVtx->vtx.x *= fSize;
			pVtx->vtx.y *= fSize;
			pVtx->vtx.z *= fSize;

			// �ʒu
			//pVtx->vtx.x += fPosX;
			//pVtx->vtx.y += fPosY;
			//pVtx->vtx.z += fPosZ;

			// ���ˌ��̐ݒ�
			pVtx->diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			// �e�N�X�`�����W�̐ݒ�
			pVtx->tex.x = fTexSize * y;
			pVtx->tex.y = fTexSize * x;
			++pVtx;
		}
	}
	//�C���f�b�N�X�z��̒��g�𖄂߂�
	std::uint32_t* pIdx = pIndexWk;
	for (std::uint32_t z = 0; z < nNumBlockY; ++z) {
		if (z > 0) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (std::uint32_t x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockY - 1) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	// ���_����
	out.m_vertexCount = nNumVertex;
	for (std::uint32_t i = 0; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(pVertexWk[i].vtx);
		out.m_vertexData.normals.push_back(pVertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(pVertexWk[i].tex);
		out.m_vertexData.colors.push_back(pVertexWk[i].diffuse);
	}

	// �C���f�b�N�X
	out.m_indexCount = nNumIndex;
	for (std::uint32_t i = 0; i < out.m_indexCount; ++i)
	{
		out.m_indexData.push_back(pIndexWk[i]);
	}

	// �ڐ��x�N�g���̐���
	CalcTangent(out);

	// �ꎞ�z��̉��
	delete[] pVertexWk;
	delete[] pIndexWk;
}

void Geometry::SkyDome(core::CoreMesh& out, int nSegment, float fTexSplit)
{
	// �v���~�e�B�u��ʐݒ�
	out.m_topology = core::PrimitiveTopology::TRIANGLE_LIST;
	float fScale = 0.5f;

	//���_�o�b�t�@�쐬
	out.m_vertexCount = (nSegment + 1) * (nSegment / 2 + 1);
	VERTEX_3D* pVertexWk = new VERTEX_3D[out.m_vertexCount];

	for (int i = 0; i <= (nSegment / 2); ++i) {
		float irad = Mathf::Pi * 2.0f / (float)nSegment * (float)i;
		float y = (float)cos(irad);
		float r = (float)sin(irad);
		float v = (float)i / (float)(nSegment / 2) * fTexSplit;
		for (int j = 0; j <= nSegment; ++j) {
			float jrad = Mathf::Pi * 2.0f / (float)nSegment * (float)j;
			float x = r * (float)cos(jrad - Mathf::Pi * 0.5f);
			float z = r * (float)sin(jrad - Mathf::Pi * 0.5f);
			float u = (float)j / (float)nSegment * fTexSplit;
			int   inx = i * (nSegment + 1) + j;
			pVertexWk[inx].vtx.x = x * -fScale;
			pVertexWk[inx].vtx.y = y * fScale;
			pVertexWk[inx].vtx.z = z * -fScale;
			pVertexWk[inx].nor.x = x;
			pVertexWk[inx].nor.y = y;
			pVertexWk[inx].nor.z = z;
			pVertexWk[inx].tex.x = u;
			pVertexWk[inx].tex.y = v;
			pVertexWk[inx].diffuse = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}

	// �C���f�b�N�X�o�b�t�@�̍쐬
	out.m_indexCount = nSegment * 3 + nSegment * (nSegment / 2 - 1) * 6 + nSegment * 3;
	std::uint32_t* pIndexWk = new std::uint32_t[out.m_indexCount];

	std::uint32_t icount = 0;
	std::uint32_t i = 0;
	for (std::uint32_t j = 0; j < nSegment; ++j) {
		pIndexWk[icount] = i * (nSegment + 1) + j;
		pIndexWk[icount + 1] = (i + 1) * (nSegment + 1) + j + 1;
		pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
		icount += 3;
	}
	for (i = 1; i < nSegment / 2; ++i) {
		for (std::uint32_t j = 0; j < nSegment; ++j) {
			pIndexWk[icount] = i * (nSegment + 1) + j;
			pIndexWk[icount + 1] = i * (nSegment + 1) + j + 1;
			pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
			icount += 3;
			pIndexWk[icount] = i * (nSegment + 1) + j + 1;
			pIndexWk[icount + 1] = (i + 1) * (nSegment + 1) + j + 1;
			pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
			icount += 3;
		}
	}
	i = nSegment / 2;
	for (std::uint32_t j = 0; j < nSegment; ++j) {
		pIndexWk[icount] = i * (nSegment + 1) + j;
		pIndexWk[icount + 1] = (i + 1) * (nSegment + 1) + j + 1;
		pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
		icount += 3;
	}

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	// ���_����
	for (std::uint32_t i = 0; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(pVertexWk[i].vtx);
		out.m_vertexData.normals.push_back(pVertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(pVertexWk[i].tex);
		out.m_vertexData.colors.push_back(pVertexWk[i].diffuse);
	}

	// �C���f�b�N�X
	for (std::uint32_t i = 0; i < out.m_indexCount; ++i)
	{
		out.m_indexData.push_back(pIndexWk[i]);
	}

	// �ꎞ�z��̉��
	delete[] pVertexWk;
	delete[] pIndexWk;
}

bool Geometry::CalcTangent(core::CoreMesh& mesh)
{
	//--- �ڃx�N�g�����v�Z
	bool useIndex = mesh.m_indexCount > 0;
	mesh.m_vertexData.tangents.resize(mesh.m_vertexCount);
	std::vector<std::uint32_t> duplicateVtxCount(mesh.m_vertexCount); // �d�����_��
	// �ʐ��̌v�Z
	std::uint32_t faceNum = 0;
	switch (mesh.m_topology)
	{
	case core::PrimitiveTopology::TRIANGLE_LIST:
		faceNum = (useIndex ? mesh.m_indexCount : mesh.m_vertexCount) / 3;
		break;
	case core::PrimitiveTopology::TRIANGLE_STRIP:
		faceNum = (useIndex ? mesh.m_indexCount : mesh.m_vertexCount) - 2;
		break;
	default:
		return false; // ���Ή�
	}

	std::vector<Vector3>& pos = mesh.m_vertexData.positions;
	std::vector<Vector2>& uv = mesh.m_vertexData.texcoord0s;

	// �ʂ��Ƃɐڃx�N�g�����v�Z
	for (std::uint32_t i = 0; i < faceNum; ++i)
	{
		// �C���f�b�N�X�̊���o��
		std::uint32_t index[3];
		if (CalcIndex(index, mesh, i) == false)
		{
			return false;
		}

		// �o���v�}�b�v�p
		Vector3 v0 = pos[index[1]] - pos[index[0]];
		Vector3 v1 = pos[index[2]] - pos[index[0]];
		Vector2 t0 = uv[index[1]] - uv[index[0]];
		Vector2 t1 = uv[index[2]] - uv[index[0]];
		// �O��
		float t = t0.x * t1.y - t1.x * t0.y;
		// �^���W�F���g
		Vector3 tangent(
			(t1.y * v0.x - t0.y * v1.x) / t,
			(t1.y * v0.y - t0.y * v1.y) / t,
			(t1.y * v0.z - t0.y * v1.z) / t
		);
		// ���K��
		tangent.Normalize();
		// �i�[
		mesh.m_vertexData.tangents[index[0]] += tangent;
		mesh.m_vertexData.tangents[index[1]] += tangent;
		mesh.m_vertexData.tangents[index[2]] += tangent;
		// �J�E���g���Z
		duplicateVtxCount[index[0]] ++;
		duplicateVtxCount[index[1]] ++;
		duplicateVtxCount[index[2]] ++;
	}

	// �d�����Őڃx�N�g���𕽋ω�
	for (std::uint32_t i = 0; i < mesh.m_vertexCount; ++i)
	{
		mesh.m_vertexData.tangents[i].x /= duplicateVtxCount[i];
		mesh.m_vertexData.tangents[i].y /= duplicateVtxCount[i];
		mesh.m_vertexData.tangents[i].z /= duplicateVtxCount[i];
	}

	return true;
}

bool Geometry::CalcIndex(std::uint32_t* pOut, core::CoreMesh& mesh, std::uint32_t face)
{
	bool useIndex = mesh.m_indexCount > 0;
	switch (mesh.m_topology)
	{
	case core::PrimitiveTopology::TRIANGLE_LIST:
		if (useIndex)
		{
			for (std::uint32_t i = 0; i < 3; ++i)
			{
				// �C���f�b�N�X�T�C�Y�ŏꍇ����
				/*switch ()
				{
				case 1: pOut[i] = reinterpret_cast<const BYTE*>(desc.pIdx)[face * 3 + i]; break;
				case 2: pOut[i] = reinterpret_cast<const WORD*>(desc.pIdx)[face * 3 + i]; break;
				case 4: pOut[i] = reinterpret_cast<const DWORD*>(desc.pIdx)[face * 3 + i]; break;
				}*/
				pOut[i] = mesh.m_indexData[face * 3 + i];
			}
		}
		else
		{
			for (std::uint32_t i = 0; i < 3; ++i)
				pOut[i] = face * 3 + i;
		}
		break;
	case core::PrimitiveTopology::TRIANGLE_STRIP:
		if (useIndex)
		{
			for (std::uint32_t i = 0; i < 3; ++i)
			{
				// �C���f�b�N�X�T�C�Y�ŏꍇ����
				/*switch (desc.idxSize)
				{
				case 1: pOut[i] = reinterpret_cast<const BYTE*>(desc.pIdx)[face + i]; break;
				case 2: pOut[i] = reinterpret_cast<const WORD*>(desc.pIdx)[face + i]; break;
				case 4: pOut[i] = reinterpret_cast<const DWORD*>(desc.pIdx)[face + i]; break;
				}*/
				pOut[i] = mesh.m_indexData[face + i];
			}
		}
		else
		{
			for (std::uint32_t i = 0; i < 3; ++i)
				pOut[i] = face + i;
		}
		break;
	default:
		return false;
	}

	return true;
}