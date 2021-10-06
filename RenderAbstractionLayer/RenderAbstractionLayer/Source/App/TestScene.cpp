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
#include <Renderer/Core/Core_RenderDevice.h>
#include <Renderer/Core/Core_RenderContext.h>

#include "Geometry.h"

core::RenderBufferID g_rdID;


 /// @brief スタート
void TestScene::Start()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

	// テクスチャの生成
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

	// シェーダー・マテリアルの生成
	core::ShaderDesc shaderDesc;
	shaderDesc.m_stages = core::ShaderStageFlags::VS | core::ShaderStageFlags::PS;

	shaderDesc.m_name = "Unlit";
	auto unlitShaderID = device->createShader(shaderDesc);
	auto unlitMatID = device->createMaterial("Unlit", unlitShaderID);
	auto* pUnlitMat = device->getMaterial(unlitMatID);
	context->setTexture(core::SHADER::SHADER_SRV_SLOT_MAINTEX, texID, core::ShaderStage::PS);

	// メッシュの生成
	auto cubeMehID = device->createMesh("cube");
	auto* pCubeMesh = device->getMesh(cubeMehID);
	Geometry::Cube(*pCubeMesh);

	// レンダーバッファの作成
	g_rdID = device->createRenderBuffer(unlitShaderID, cubeMehID);

}

/// @brief システムの更新
void TestScene::Update()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

}

/// @brief パイプラインの描画
void TestScene::Render()
{
	auto* renderer = m_pSceneManager->getEngine()->getRenderer();
	auto* device = renderer->getDevice();
	auto* context = renderer->getContext();

	// システムバッファ送信

	// トランスフォームバッファ送信

	// マテリアルの指定

	// レンダーバッファの指定

	// 描画
	context->render(g_rdID);
}

/// @brief エンド
void TestScene::End()
{

}
