/*****************************************************************//**
 * \file   TestScene.h
 * \brief  テストシーン
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

// ポストエフェクト用
core::ShaderID			g_postShaderID;
core::MaterialID			g_postMatID;
core::RenderBufferID		g_postRdID;

constexpr int MAX_WORLD = 512;

 /// @brief スタート
void TestScene::Start()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

	//--- ジオメトリ ---
	{
		// テクスチャの生成
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

		// シェーダー・マテリアルの生成
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

		// メッシュの生成
		auto cubeMehID = device->createMesh("cube");
		auto* pCubeMesh = device->getMesh(cubeMehID);
		Geometry::Cube(*pCubeMesh);

		// レンダーバッファの作成
		g_rdID = device->createRenderBuffer(unlitShaderID, cubeMehID);
	}

	// レンダーターゲットの生成
	core::TextureDesc rtDesc = {};
	rtDesc.name = "レンダーターゲット";
	rtDesc.width = width;
	rtDesc.height = height;
	rtDesc.bindFlags = 0 | core::BindFlags::RENDER_TARGET | core::BindFlags::SHADER_RESOURCE;
	rtDesc.format = core::TextureFormat::R8G8B8A8_UNORM;
	g_rtID = device->createRenderTarget(rtDesc, g_clearColor);
	auto* pRT = device->getRenderTarget(g_rtID);

	// デプスステンシルの生成
	rtDesc.name = "デプスステンシル";
	rtDesc.bindFlags = 0 | core::BindFlags::DEPTH_STENCIL | core::BindFlags::SHADER_RESOURCE;
	rtDesc.format = core::TextureFormat::R32_TYPELESS;
	g_dsID = device->createDepthStencil(rtDesc);

	// ワールドマトリックスの作成
	core::BufferDesc bufferDesc;
	bufferDesc.name = "WorldMatrix";
	bufferDesc.size = sizeof(Matrix);
	bufferDesc.count = MAX_WORLD;
	bufferDesc.bindFlags = 0 | core::BindFlags::CONSTANT_BUFFER;
	bufferDesc.usage = core::Usage::DEFAULT;
	bufferDesc.cpuAccessFlags = 0 | core::CPUAccessFlags::NONE;
	g_worldID = device->createBuffer(bufferDesc);

	//----- ポストエフェクト用
	{
		// シェーダー・マテリアルの生成
		core::ShaderDesc shaderDesc;
		shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;
		shaderDesc.m_name = "Post";

		g_postShaderID = device->createShader(shaderDesc);
		g_postMatID = device->createMaterial("Post", g_postShaderID);
		auto* pPostMat = device->getMaterial(g_postMatID);
		pPostMat->setVector3("_Color", Vector3(1, 1, 1));
		pPostMat->setTexture("_RT", pRT->m_texID);
		pPostMat->setSampler("_Sampler", core::SamplerState::LINEAR_WRAP);

		// メッシュの生成
		auto quadID = device->createMesh("quad");
		auto* pQuad = device->getMesh(quadID);
		Geometry::Quad(*pQuad);

		// レンダーバッファの作成
		g_postRdID = device->createRenderBuffer(g_postShaderID, quadID);
	}
}

/// @brief システムの更新
void TestScene::Update()
{

}

/// @brief パイプラインの描画
void TestScene::Render()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* cmdList = renderer->getCommandList();

	float width = static_cast<float>(renderer->getCoreEngine()->getWindowWidth());
	float height = static_cast<float>(renderer->getCoreEngine()->getWindowHeight());

	auto* pRT = device->getRenderTarget(g_rtID);
	auto* pUnlitMat = device->getMaterial(g_matID);
	auto* pPostMat = device->getMaterial(g_postMatID);
	auto* pWorldBuffer = device->getBuffer(g_worldID);

	// カメラ
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
	pUnlitMat->setMatrix("_mView", view);
	pUnlitMat->setMatrix("_mProj", proj);


	// ワールドマトリックス
	static float angleY = 0;
	angleY += 0.01f;
	float y = -8;
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

	//----- 描画
	{
		// レンダーターゲット指定
		cmdList->setRenderTarget(g_rtID, g_dsID);

		// レンダーターゲットクリア
		cmdList->clearRederTarget(g_rtID, g_clearColor);
		cmdList->clearDepthStencil(g_dsID, 1.0f, 0);

		// ビューポート指定
		cmdList->setViewport(Viewport(0, 0, width, height));

		// マテリアルの指定
		cmdList->setMaterial(g_matID);

		// レンダーバッファの指定
		cmdList->setRenderBuffer(g_rdID);

		// バッファ指定
		cmdList->bindGlobalBuffer(g_shaderID, "World", g_worldID);

		// 描画
		//for (int i = 0; i < 100; ++i)
		{
			cmdList->render(g_rdID, MAX_WORLD);
		}
	}

	//--- ポストエフェクト
	{
		// トランスフォームのセット
		Matrix world = Matrix::CreateScale(width, height, 1.0f);
		world = world.Transpose();
		proj = Matrix::CreateOrtho(width, height, 1.0f, 100.0f);
		proj = proj.Transpose();

		pPostMat->setMatrix("_mWorld", world);
		pPostMat->setMatrix("_mView", view);
		pPostMat->setMatrix("_mProj", proj);

		// マテリアルの更新
		pPostMat->setVector3("_Color", Vector3(1, 1, 1));
		static float _time;
		_time += 0.1f;
		pPostMat->setFloat("_Time", _time);


		// バッファバッファ指定
		cmdList->setBackBuffer();
		cmdList->clearBackBuffer(Color());

		// マテリアルの指定
		cmdList->setMaterial(g_postMatID);
		// レンダーバッファの指定
		cmdList->setRenderBuffer(g_postRdID);
		// 描画
		cmdList->render(g_postRdID, 1);
	}


	// バックバッファへコピー
	//cmdList->copyBackBuffer(pRT->m_texID);

}

/// @brief エンド
void TestScene::End()
{

}
