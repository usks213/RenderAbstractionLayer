/*****************************************************************//**
 * \file   TestScene.h
 * \brief  �e�X�g�V�[��
 *
 * \author USAMI KOSHI
 * \date   2021/10/06
 *********************************************************************/

#include "TestScene.h"

#include <Engine/Core_Engine.h>
#include <Renderer/Core/Core_Renderer.h>
#include <Renderer/Core/Core_RenderDevice.h>
#include <Renderer/Core/Core_RenderContext.h>

#include "Geometry.h"

core::MaterialID g_matID;
core::RenderBufferID g_rdID;


 /// @brief �X�^�[�g
void TestScene::Start()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

	//// �e�N�X�`���̐���
	//uint32_t texWidth = 256u;
	//uint32_t texHeight = 256u;
	//uint32_t texDepth = 4u;
	//std::vector<unsigned char> texArray(texWidth * texHeight * texDepth);
	//core::TextureData texData;
	//texData.pInitData = texArray.data();
	//texData.size = texArray.size();

	//uint32_t texX = texWidth * texDepth;
	//for (int y = 0; y < texHeight; ++y)
	//{
	//	for (int x = 0; x < texX; x += texDepth)
	//	{
	//		if (y < texHeight / 2)
	//		{
	//			if (x < texX / 2)
	//			{
	//				texArray[y * texX + x + 3] = 255;	// R
	//				texArray[y * texX + x + 2] = 0;		// G
	//				texArray[y * texX + x + 1] = 0;		// B
	//				texArray[y * texX + x + 0] = 255;	// A
	//			}
	//			else
	//			{
	//				texArray[y * texX + x + 3] = 0;		// R
	//				texArray[y * texX + x + 2] = 255;	// G
	//				texArray[y * texX + x + 1] = 0;		// B
	//				texArray[y * texX + x + 0] = 255;	// A
	//			}
	//		}
	//		else
	//		{
	//			if (x < texX / 2)
	//			{
	//				texArray[y * texX + x + 3] = 0;		// R
	//				texArray[y * texX + x + 2] = 0;		// G
	//				texArray[y * texX + x + 1] = 255;	// B
	//				texArray[y * texX + x + 0] = 255;	// A
	//			}
	//			else
	//			{
	//				texArray[y * texX + x + 3] = 255;	// R
	//				texArray[y * texX + x + 2] = 255;	// G
	//				texArray[y * texX + x + 1] = 255;	// B
	//				texArray[y * texX + x + 0] = 255;	// A
	//			}
	//		}

	//	}
	//}

	//core::TextureDesc texDesc;
	//texDesc.width = texWidth;
	//texDesc.height = texHeight;
	//texDesc.depth = texDepth;
	//texDesc.mipLevels = 1;
	//texDesc.bindFlags = 0 | core::BindFlags::SHADER_RESOURCE;
	//texDesc.format = core::TextureFormat::R8G8B8A8_UNORM;

	//auto texID = device->createTexture(texDesc, &texData);

	// �V�F�[�_�[�E�}�e���A���̐���
	core::ShaderDesc shaderDesc;
	shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;

	shaderDesc.m_name = "Unlit";

	auto unlitShaderID = device->createShader(shaderDesc);
	auto unlitMatID = device->createMaterial("Unlit", unlitShaderID);
	auto* pUnlitMat = device->getMaterial(unlitMatID);
	pUnlitMat->setVector4("_Color", Vector4(1, 1, 1, 1));
	//pUnlitMat->setTexture("_MainTexture", texID);

	//context->setTexture(core::SHADER::SHADER_SRV_SLOT_MAINTEX, texID, core::ShaderStage::PS);
	g_matID = unlitMatID;

	// ���b�V���̐���
	auto cubeMehID = device->createMesh("cube");
	auto* pCubeMesh = device->getMesh(cubeMehID);
	Geometry::Cube(*pCubeMesh);

	// �����_�[�o�b�t�@�̍쐬
	g_rdID = device->createRenderBuffer(unlitShaderID, cubeMehID);

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

}

/// @brief �V�X�e���̍X�V
void TestScene::Update()
{
	//auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	//auto* device = renderer->getDevice();
	//auto* context = renderer->getContext();

}

/// @brief �p�C�v���C���̕`��
void TestScene::Render()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

	auto* pUnlitMat = device->getMaterial(g_matID);

	// �V�X�e���o�b�t�@���M
	Vector3 eyepos = Vector3(0, 2, -5);
	Vector3 eyedir = Vector3(0, 0, 0);
	Vector3 up = Vector3(0, 1, 0);
	Matrix view = Matrix::CreateLookAt(eyepos, eyedir, up);

	Matrix proj = Matrix::CreatePerspectiveFOV(
		Mathf::ToRadians(60),
		width,
		height,
		1.0f,
		10.0f
	);

	core::SHADER::SystemBuffer systemBuffer;
	systemBuffer._mView = view.Transpose();
	systemBuffer._mProj = proj.Transpose();

	//context->sendSystemBuffer(systemBuffer);

	// �g�����X�t�H�[���o�b�t�@���M
	static float angleY = 0;
	angleY += 0.01f;
	Vector3 pos = Vector3(0, 0, 0);
	Vector3 rot = Vector3(0, angleY, 0);
	Vector3 sca = Vector3(1, 1, 1);
	Matrix world = Matrix::CreateScale(sca);
	world *= Matrix::CreateRotationZXY(rot);
	world *= Matrix::CreateTranslation(pos);

	world = world.Transpose();
	view = view.Transpose();
	proj = proj.Transpose();

	//context->sendTransformBuffer(world);
	pUnlitMat->setMatrix("_mWorld", world);
	pUnlitMat->setMatrix("_mView", view);
	pUnlitMat->setMatrix("_mProj", proj);

	// �}�e���A���̎w��
	//context->setMaterial(g_matID);

	// �����_�[�o�b�t�@�̎w��
	//context->setRenderBuffer(g_rdID);

	context->setPipelineState(g_matID, g_rdID);

	// �`��
	context->render(g_rdID);
}

/// @brief �G���h
void TestScene::End()
{

}
