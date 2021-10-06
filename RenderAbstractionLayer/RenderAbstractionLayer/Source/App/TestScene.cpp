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

core::RenderBufferID g_rdID;


 /// @brief �X�^�[�g
void TestScene::Start()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

	// �e�N�X�`���̐���
	uint32_t texWidth = 256u;
	uint32_t texHeight = 256u;
	uint32_t texDepth = 4u;
	std::vector<unsigned char> texArray(texWidth * texHeight * texDepth);
	core::TextureData texData;
	texData.pInitData = texArray.data();
	texData.size = texArray.size();

	for (int i = 0; i < texArray.size(); i += texDepth)
	{
		if (i < texArray.size() / 2)
		{
			texArray[i + 0] = 255;
			texArray[i + 1] = 0;
			texArray[i + 2] = 0;
			texArray[i + 3] = 255;
		}
		else
		{
			texArray[i + 0] = 255;
			texArray[i + 1] = 0;
			texArray[i + 2] = 255;
			texArray[i + 3] = 255;
		}
	}

	core::TextureDesc texDesc;
	texDesc.width = texWidth;
	texDesc.height = texHeight;
	texDesc.depth = texDepth;
	texDesc.mipLevels = 1;
	texDesc.bindFlags = 0 | core::BindFlags::SHADER_RESOURCE;
	texDesc.format = core::TextureFormat::R8G8B8A8_UNORM;

	auto texID = device->createTexture(texDesc, &texData);

	// �V�F�[�_�[�E�}�e���A���̐���
	core::ShaderDesc shaderDesc;
	shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;

	shaderDesc.m_name = "Unlit";
	auto unlitShaderID = device->createShader(shaderDesc);
	auto unlitMatID = device->createMaterial("Unlit", unlitShaderID);
	auto* pUnlitMat = device->getMaterial(unlitMatID);
	context->setTexture(core::SHADER::SHADER_SRV_SLOT_MAINTEX, texID, core::ShaderStage::PS);

	// ���b�V���̐���
	auto cubeMehID = device->createMesh("cube");
	auto* pCubeMesh = device->getMesh(cubeMehID);
	Geometry::Cube(*pCubeMesh);

	// �����_�[�o�b�t�@�̍쐬
	g_rdID = device->createRenderBuffer(unlitShaderID, cubeMehID);

}

/// @brief �V�X�e���̍X�V
void TestScene::Update()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

}

/// @brief �p�C�v���C���̕`��
void TestScene::Render()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

	// �V�X�e���o�b�t�@���M

	// �g�����X�t�H�[���o�b�t�@���M

	// �}�e���A���̎w��

	// �����_�[�o�b�t�@�̎w��

	// �`��
	context->render(g_rdID);
}

/// @brief �G���h
void TestScene::End()
{

}
