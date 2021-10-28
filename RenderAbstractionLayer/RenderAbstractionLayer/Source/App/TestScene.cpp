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
Color					g_clearColor = Color(0.2f, 0.2f, 0.2f, 1.0f);

// ���C�e�B���O
constexpr std::uint32_t	MAX_POINT_LIGHT = 3;
core::CorePointLight		g_pointLights[MAX_POINT_LIGHT];
core::BufferID			g_pointLightBufferID;
core::ShaderID			g_lightShaderID;
core::MaterialID			g_lightMatID;
core::RenderBufferID		g_litRdID;


// �|�X�g�G�t�F�N�g�p
core::ShaderID			g_postShaderID;
core::MaterialID			g_postMatID;
core::RenderBufferID		g_postRdID;

constexpr int MAX_WORLD = 512;

 /// @brief �X�^�[�g
void TestScene::Start()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

	//--- �W�I���g�� ---
	auto quadID = device->createMesh("quad");
	auto* pQuad = device->getMesh(quadID);
	Geometry::Quad(*pQuad);

	auto cubeMehID = device->createMesh("cube");
	auto* pCubeMesh = device->getMesh(cubeMehID);
	Geometry::Cube(*pCubeMesh);

	{
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
		g_matID = unlitMatID;

		// �����_�[�o�b�t�@�̍쐬
		g_rdID = device->createRenderBuffer(unlitShaderID, cubeMehID);
	}

	{
		// �����_�[�^�[�Q�b�g�̐���
		core::TextureDesc rtDesc = {};
		rtDesc.name = "�����_�[�^�[�Q�b�g";
		rtDesc.width = width;
		rtDesc.height = height;
		rtDesc.bindFlags = 0 | core::BindFlags::RENDER_TARGET | core::BindFlags::SHADER_RESOURCE;
		rtDesc.format = core::TextureFormat::R8G8B8A8_UNORM;
		g_rtID = device->createRenderTarget(rtDesc, g_clearColor);

		// �f�v�X�X�e���V���̐���
		rtDesc.name = "�f�v�X�X�e���V��";
		rtDesc.bindFlags = 0 | core::BindFlags::DEPTH_STENCIL | core::BindFlags::SHADER_RESOURCE;
		rtDesc.format = core::TextureFormat::R32_TYPELESS;
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

	auto* pRT = device->getRenderTarget(g_rtID);
	//----- �|�X�g�G�t�F�N�g�p
	{
		// �V�F�[�_�[�E�}�e���A���̐���
		core::ShaderDesc shaderDesc;
		shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;
		shaderDesc.m_name = "Post";

		g_postShaderID = device->createShader(shaderDesc);
		g_postMatID = device->createMaterial("Post", g_postShaderID);
		auto* pPostMat = device->getMaterial(g_postMatID);
		pPostMat->setVector3("_Color", Vector3(1, 1, 1));
		pPostMat->setTexture("_RT", pRT->m_texID);
		pPostMat->setSampler("_Sampler", core::SamplerState::LINEAR_WRAP);

		// �����_�[�o�b�t�@�̍쐬
		g_postRdID = device->createRenderBuffer(g_postShaderID, quadID);
	}

	//----- ���C�e�B���O -----
	{
		// �V�F�[�_�[�E�}�e���A���̐���
		core::ShaderDesc shaderDesc;
		shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;
		shaderDesc.m_name = "Lit";

		g_lightShaderID = device->createShader(shaderDesc);
		g_lightMatID = device->createMaterial("Lit", g_lightShaderID);
		auto* pMat = device->getMaterial(g_lightMatID);
		pMat->setVector3("_Color", Vector3(1, 1, 1));
		pMat->setFloat("_PointLightNum", MAX_POINT_LIGHT);
		pMat->setSampler("_Sampler", core::SamplerState::LINEAR_WRAP);
		pMat->setTexture("_Texture", g_texID);

		// �|�C���g���C�g�o�b�t�@�쐬
		core::BufferDesc bufferDesc;
		bufferDesc.name = "PointLights";
		bufferDesc.size = sizeof(core::CorePointLight);
		bufferDesc.count = MAX_POINT_LIGHT;
		bufferDesc.bindFlags = 0 | core::BindFlags::SHADER_RESOURCE;
		bufferDesc.usage = core::Usage::DEFAULT;
		bufferDesc.cpuAccessFlags = 0 | core::CPUAccessFlags::NONE;
		bufferDesc.miscFlags = 0 | core::MiscFlags::BUFFER_STRUCTURED;
		g_pointLightBufferID = device->createBuffer(bufferDesc);

		// �����_�[�o�b�t�@�̍쐬
		g_litRdID = device->createRenderBuffer(g_lightShaderID, cubeMehID);
	}
}

/// @brief �V�X�e���̍X�V
void TestScene::Update()
{

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
	auto* pLitMat = device->getMaterial(g_lightMatID);
	auto* pPostMat = device->getMaterial(g_postMatID);

	auto* pWorldBuffer = device->getBuffer(g_worldID);
	auto* pPointLightBuffer = device->getBuffer(g_pointLightBufferID);

	// �J����
	Vector3 eyepos = Vector3(0, 3, -7);
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
	pUnlitMat->setMatrix("_mView", view);
	pUnlitMat->setMatrix("_mProj", proj);
	pLitMat->setMatrix("_mView", view);
	pLitMat->setMatrix("_mProj", proj);

	// ���[���h�}�g���b�N�X
	static float angleY = 0;
	angleY += 0.01f;
	float y = -8;
	std::vector<Matrix> aWorld(MAX_WORLD);
	for (int i = 0; i < MAX_WORLD; ++i)
	{
		if (i % 40 == 0)
			y += 1.0f;
		auto& world = aWorld[i];
		Vector3 pos;// = Vector3(-10 + i % 40 * 0.5f, y, 0);
		Vector3 rot = Vector3(0, angleY, 0);
		//Vector3 sca = Vector3(0.3f, 0.3f, 0.3f);
		Vector3 sca = Vector3(1.0f, 1.0f, 1.0f);
		world = Matrix::CreateScale(sca);
		world *= Matrix::CreateRotationZXY(rot);
		world *= Matrix::CreateTranslation(pos);
		world = world.Transpose();
	}
	pWorldBuffer->UpdateBuffer(aWorld.data(), sizeof(Matrix) * MAX_WORLD);

	//----- ���C�g�X�V
	{
		Vector3 pointPos = Vector3(3, 0, 0);

		for (std::uint32_t i = 0; i < MAX_POINT_LIGHT; ++i)
		{
			g_pointLights[i].position = Vector3::Transform(pointPos, Matrix::CreateRotationY(
				angleY + Mathf::TwoPi / MAX_POINT_LIGHT * (i % MAX_POINT_LIGHT)));
			g_pointLights[i].range = 10;
			g_pointLights[i].color = Vector4(1, 1, 1, 1);
		}
		pPointLightBuffer->UpdateBuffer(g_pointLights, sizeof(core::CorePointLight) * MAX_POINT_LIGHT);
	}


	//----- �`��
	{
		// �����_�[�^�[�Q�b�g�w��
		cmdList->setRenderTarget(g_rtID, g_dsID);

		// �����_�[�^�[�Q�b�g�N���A
		cmdList->clearRederTarget(g_rtID, g_clearColor);
		cmdList->clearDepthStencil(g_dsID, 1.0f, 0);

		// �r���[�|�[�g�w��
		cmdList->setViewport(Viewport(0, 0, width, height));

		// �}�e���A���̎w��
		cmdList->setMaterial(g_matID);
		//cmdList->setMaterial(g_lightMatID);

		// �����_�[�o�b�t�@�̎w��
		cmdList->setRenderBuffer(g_rdID);
		//cmdList->setRenderBuffer(g_litRdID);

		// �o�b�t�@�w��
		cmdList->bindGlobalBuffer(g_shaderID, "World", g_worldID);
		//cmdList->bindGlobalBuffer(g_lightShaderID, "World", g_worldID);
		//cmdList->bindGlobalBuffer(g_lightShaderID, "_PointLights", g_pointLightBufferID);

		// �`��
		//for (int i = 0; i < 100; ++i)
		{
			cmdList->render(g_rdID, 1);
			//cmdList->render(g_litRdID, 1);
		}
	}

	//--- �|�X�g�G�t�F�N�g
	{
		// �g�����X�t�H�[���̃Z�b�g
		Matrix world = Matrix::CreateScale(width, height, 1.0f);
		world = world.Transpose();
		view = Matrix::CreateLookAt(Vector3(0,0,-1), Vector3(), up);
		view = view.Transpose();
		proj = Matrix::CreateOrtho(width, height, 1.0f, 100.0f);
		proj = proj.Transpose();

		pPostMat->setMatrix("_mWorld", world);
		pPostMat->setMatrix("_mView", view);
		pPostMat->setMatrix("_mProj", proj);

		// �}�e���A���̍X�V
		pPostMat->setVector3("_Color", Vector3(1, 1, 1));
		static float _time;
		_time += 0.1f;
		pPostMat->setFloat("_Time", _time);


		// �o�b�t�@�o�b�t�@�w��
		cmdList->setBackBuffer();
		cmdList->clearBackBuffer(Color());

		// �}�e���A���̎w��
		cmdList->setMaterial(g_postMatID);
		// �����_�[�o�b�t�@�̎w��
		cmdList->setRenderBuffer(g_postRdID);
		// �`��
		cmdList->render(g_postRdID, 1);
	}


	// �o�b�N�o�b�t�@�փR�s�[
	//cmdList->copyBackBuffer(pRT->m_texID);

}

/// @brief �G���h
void TestScene::End()
{

}
