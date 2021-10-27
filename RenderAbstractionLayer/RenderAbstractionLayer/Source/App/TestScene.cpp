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
#include <Renderer/Core/Core_Device.h>
#include <Renderer/Core/Core_CommandList.h>

#include "Geometry.h"

core::ShaderID			g_shaderID;
core::MaterialID			g_matID;
core::RenderBufferID		g_rdID;
core::TextureID			g_texID;
core::BufferID			g_worldID;

core::RenderTargetID		g_rtID;
core::DepthStencilID		g_dsID;

constexpr int MAX_WORLD = 512;

 /// @brief �X�^�[�g
void TestScene::Start()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();

	// �e�N�X�`���̐���
	uint32_t texWidth = 256u;
	uint32_t texHeight = 256u;
	uint32_t texDepth = 4u;
	std::vector<unsigned char> texArray(texWidth * texHeight * texDepth);
	core::TextureData texData;
	texData.pInitData = texArray.data();
	texData.size = texArray.size();

	uint32_t texX = texWidth * texDepth;
	for (int y = 0; y < texHeight; ++y)
	{
		for (int x = 0; x < texX; x += texDepth)
		{
			if (y < texHeight / 2)
			{
				if (x < texX / 2)
				{
					texArray[y * texX + x + 3] = 255;	// R
					texArray[y * texX + x + 2] = 0;		// G
					texArray[y * texX + x + 1] = 0;		// B
					texArray[y * texX + x + 0] = 255;	// A
				}
				else
				{
					texArray[y * texX + x + 3] = 0;		// R
					texArray[y * texX + x + 2] = 255;	// G
					texArray[y * texX + x + 1] = 0;		// B
					texArray[y * texX + x + 0] = 255;	// A
				}
			}
			else
			{
				if (x < texX / 2)
				{
					texArray[y * texX + x + 3] = 0;		// R
					texArray[y * texX + x + 2] = 0;		// G
					texArray[y * texX + x + 1] = 255;	// B
					texArray[y * texX + x + 0] = 255;	// A
				}
				else
				{
					texArray[y * texX + x + 3] = 255;	// R
					texArray[y * texX + x + 2] = 255;	// G
					texArray[y * texX + x + 1] = 255;	// B
					texArray[y * texX + x + 0] = 255;	// A
				}
			}

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
	g_texID = texID;

	// �V�F�[�_�[�E�}�e���A���̐���
	core::ShaderDesc shaderDesc;
	shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;

	shaderDesc.m_name = "Unlit";

	auto unlitShaderID = device->createShader(shaderDesc);
	auto unlitMatID = device->createMaterial("Unlit", unlitShaderID);
	auto* pUnlitMat = device->getMaterial(unlitMatID);
	pUnlitMat->setVector4("_Color", Vector4(1, 1, 1, 1));
	pUnlitMat->setTexture("_Texture", texID);
	pUnlitMat->setSampler("_Sampler", core::SamplerState::LINEAR_WRAP);
	g_shaderID = unlitShaderID;

	//cmdList->setTexture(core::SHADER::SHADER_SRV_SLOT_MAINTEX, texID, core::ShaderStage::PS);
	g_matID = unlitMatID;

	// ���b�V���̐���
	auto cubeMehID = device->createMesh("cube");
	auto* pCubeMesh = device->getMesh(cubeMehID);
	Geometry::Cube(*pCubeMesh);

	// �����_�[�o�b�t�@�̍쐬
	g_rdID = device->createRenderBuffer(unlitShaderID, cubeMehID);

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

	// �����_�[�^�[�Q�b�g�̐���
	core::TextureDesc rtDesc = {};
	rtDesc.name = "�����_�[�^�[�Q�b�g";
	rtDesc.width = width;
	rtDesc.height = height;
	rtDesc.bindFlags = 0 | core::BindFlags::RENDER_TARGET;
	rtDesc.format = core::TextureFormat::R8G8B8A8_UNORM;
	g_rtID = device->createRenderTarget(rtDesc);

	// �f�v�X�X�e���V���̐���
	rtDesc.name = "�f�v�X�X�e���V��";
	rtDesc.bindFlags = 0 | core::BindFlags::DEPTH_STENCIL;
	rtDesc.format = core::TextureFormat::D32_FLOAT;
	g_dsID = device->createDepthStencil(rtDesc);

	// ���[���h�}�g���b�N�X�̍쐬
	core::BufferDesc bufferDesc;
	bufferDesc.name = "WorldMatrix";
	bufferDesc.size = sizeof(Matrix);
	bufferDesc.count = MAX_WORLD;
	bufferDesc.bindFlags = 0 | core::BindFlags::CONSTANT_BUFFER;
	bufferDesc.usage = core::Usage::DEFAULT;
	bufferDesc.cpuAccessFlags = 0 | core::CPUAccessFlags::NONE;

	g_worldID = device->createBuffer(bufferDesc);
}

/// @brief �V�X�e���̍X�V
void TestScene::Update()
{
	//auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	//auto* device = renderer->getDevice();
	//auto* cmdList = renderer->getCommandList();

}

/// @brief �p�C�v���C���̕`��
void TestScene::Render()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* cmdList = renderer->getCommandList();

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

	auto* pRT = device->getRenderTarget(g_rtID);
	auto* pUnlitMat = device->getMaterial(g_matID);
	auto* pWorldBuffer = device->getBuffer(g_worldID);

	// �V�X�e���o�b�t�@���M
	Vector3 eyepos = Vector3(0, 0, -15);
	Vector3 eyedir = Vector3(0, 0, 0);
	Vector3 up = Vector3(0, 1, 0);
	Matrix view = Matrix::CreateLookAt(eyepos, eyedir, up);

	Matrix proj = Matrix::CreatePerspectiveFOV(
		Mathf::ToRadians(60),
		width,
		height,
		1.0f,
		100.0f
	);
	view = view.Transpose();
	proj = proj.Transpose();

	core::SHADER::SystemBuffer systemBuffer;
	systemBuffer._mView = view.Transpose();
	systemBuffer._mProj = proj.Transpose();

	//cmdList->sendSystemBuffer(systemBuffer);
	static float angleY = 0;
	angleY += 0.01f;
	float y = -8;
	// �g�����X�t�H�[���o�b�t�@���M
	std::vector<Matrix> aWorld(MAX_WORLD);
	for (int i = 0; i < MAX_WORLD; ++i)
	{
		if (i % 40 == 0)
			y += 1.0f;
		auto& world = aWorld[i];
		Vector3 pos = Vector3(-10 + i % 40 * 0.5f, y, 0);
		Vector3 rot = Vector3(0, angleY, 0);
		Vector3 sca = Vector3(0.3f, 0.3f, 0.3f);
		world = Matrix::CreateScale(sca);
		world *= Matrix::CreateRotationZXY(rot);
		world *= Matrix::CreateTranslation(pos);
		world = world.Transpose();
	}

	pWorldBuffer->UpdateBuffer(aWorld.data(), sizeof(Matrix) * MAX_WORLD);
	//cmdList->sendTransformBuffer(world);
	//pUnlitMat->setMatrix("_mWorld", world);
	pUnlitMat->setMatrix("_mView", view);
	pUnlitMat->setMatrix("_mProj", proj);
	//pUnlitMat->setTexture("_Texture", g_texID);

	// �r���[�|�[�g�쐬
	Viewport viewport = Viewport(0, 0, width, height);

	//----- �`��

	// �����_�[�^�[�Q�b�g�w��
	//cmdList->setBackBuffer();
	cmdList->setRenderTarget(g_rtID, g_dsID);

	// �����_�[�^�[�Q�b�g�N���A
	//cmdList->clearBackBuffer(Color(0.2f, 0.2f, 0.2f, 1.0f));
	cmdList->clearRederTarget(g_rtID, Color(0.2f, 0.2f, 0.2f, 1.0f));
	cmdList->clearDepthStencil(g_dsID, 1.0f, 0);

	// �r���[�|�[�g�w��
	cmdList->setViewport(viewport);

	// �}�e���A���̎w��
	cmdList->setMaterial(g_matID);

	// �����_�[�o�b�t�@�̎w��
	cmdList->setRenderBuffer(g_rdID);

	// �o�b�t�@�w��
	cmdList->bindGlobalBuffer(g_shaderID, "World", g_worldID);

	// �`��
	//for (int i = 0; i < 100; ++i)
	{
		cmdList->render(g_rdID, MAX_WORLD);
	}

	// �o�b�N�o�b�t�@�փR�s�[
	cmdList->copyBackBuffer(pRT->m_texID);

}

/// @brief �G���h
void TestScene::End()
{

}
