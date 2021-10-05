/*****************************************************************//**
 * \file   D3D11_Shader.h
 * \brief  DirectX11シェーダー
 *
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/

#include "D3D11_Shader.h"
#include <Renderer/Core/Core_ShaderResource.h>
#include <d3dcompiler.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace d3d11;

namespace {
	/// @brief シェーダファイルパス
	const char* SHADER_FILE_PATH = "data/shader/";

	/// @brief エントリーポイント、ファイル識別子
	const char* SHADER_TYPES[] = {
		"VS","GS","DS","HS","PS","CS",
	};
	/// @brief HLSLバージョン
	const char* HLSL_VERS[] = {
		"vs_5_0",
		"gs_5_0",
		"ds_5_0",
		"hs_5_0",
		"ps_5_0",
		"cs_5_0",
	};
}

/// @brief  コンストラクタ
/// @param device デバイス
/// @param desc シェーダ情報
D3D11Shader::D3D11Shader(ID3D11Device1* device, core::ShaderDesc desc, const core::ShaderID& id) :
	core::CoreShader(desc, id),
	m_comShaders(static_cast<size_t>(core::ShaderStage::MAX)),
	m_inputLayout(),
	vs(nullptr),
	gs(nullptr),
	ds(nullptr),
	hs(nullptr),
	ps(nullptr),
	cs(nullptr)
{
	// 初期化
	for (auto* shader : shaders) {
		shader = nullptr;
	}

	// コンパイルしたシェーダデータ
	std::vector<ComPtr<ID3DBlob>>				blobs(static_cast<size_t>(core::ShaderStage::MAX));
	// シェーダリフレクション
	std::vector<ComPtr<ID3D11ShaderReflection>>	reflections(static_cast<size_t>(core::ShaderStage::MAX));
	// シェーダ情報一時格納用
	D3D11_SHADER_DESC							shaderDesc = {};


	// シェーダステージ数だけコンパイルを試す
	for (auto stage = core::ShaderStage::VS; stage < core::ShaderStage::MAX; ++stage)
	{
		// ステージがない場合はスキップ
		if (!hasStaderStage(desc.m_stages, stage)) continue;

		ComPtr<ID3DBlob>& blob = blobs[static_cast<size_t>(stage)];
		ComPtr<ID3D11ShaderReflection>& reflection = reflections[static_cast<size_t>(stage)];

		// パス
		std::string filepath = SHADER_FILE_PATH + desc.m_name + "_" + 
			SHADER_TYPES[static_cast<size_t>(stage)] + ".hlsl";
		std::wstring wfilepath = std::wstring(filepath.begin(), filepath.end());
		// エラー
		ID3DBlob* d3dError = nullptr;
		// ストリーム
		std::ifstream ifsShader(filepath, std::ios::in | std::ios::binary | std::ios::ate);
		if (ifsShader.good())
		{
			// ソースファイルからコンパイル
			CHECK_FAILED(D3DCompileFromFile(
				wfilepath.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				SHADER_TYPES[static_cast<size_t>(stage)],
				HLSL_VERS[static_cast<size_t>(stage)],
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				blob.GetAddressOf(),
				&d3dError));
			// エラーメッセージ!!


			// 読み込み成功
			if (blob)
			{
				// シェーダオブジェクト生成
				createShaderObjct(device, stage, blob);

				// シェーダリフレクション取得
				void** ppBuffer = reinterpret_cast<void**>(reflection.GetAddressOf());
				CHECK_FAILED(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(),
					IID_ID3D11ShaderReflection, ppBuffer));
			}

		}
		else
		{
			// シェーダファイルなし!!

		}
	}

	// 頂点シェーダがある場合はインプットレイアウトを作成
	auto& vsReflection = reflections[static_cast<size_t>(core::ShaderStage::VS)];
	if (vsReflection)
	{
		auto& vsBlob = blobs[static_cast<size_t>(core::ShaderStage::VS)];
		vsReflection->GetDesc(&shaderDesc);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts(shaderDesc.InputParameters);

		// 入力レイアウト数分
		m_inputLayoutVariableList.resize(shaderDesc.InputParameters);
		for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC signature;
			vsReflection->GetInputParameterDesc(i, &signature);

			// インプットレイアウト要素
			D3D11_INPUT_ELEMENT_DESC inputDesc;
			inputDesc.SemanticName = signature.SemanticName;
			inputDesc.SemanticIndex = signature.SemanticIndex;
			//inputDesc.Format = 後..
			inputDesc.InputSlot = 0;
			inputDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			inputDesc.InstanceDataStepRate = 0;

			// 入力レイアウト情報
			m_inputLayoutVariableList[i].semanticName = inputDesc.SemanticName;
			m_inputLayoutVariableList[i].semanticIndex = inputDesc.SemanticIndex;
			if (i > 0)
			{
				m_inputLayoutVariableList[i].offset =
					static_cast<std::size_t>(m_inputLayoutVariableList[i - 1].formatSize) * sizeof(float) +
					m_inputLayoutVariableList[i - 1].offset;
			}

			// ビットマスクでフォーマットを分ける
			if (signature.Mask == 0x01) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32 : inputDesc.Format = DXGI_FORMAT_R32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32 : inputDesc.Format = DXGI_FORMAT_R32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
				
			}
			else if (signature.Mask <= 0x03) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}
			else if (signature.Mask <= 0x07) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32B32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}
			else if (signature.Mask <= 0x0F) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32B32A32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}

			// 入力レイアウト作成用情報格納
			inputLayouts[i] = inputDesc;
		}

		// 入力レイアウト情報作成
		if (!inputLayouts.empty()){
			CHECK_FAILED(device->CreateInputLayout(
				inputLayouts.data(),
				shaderDesc.InputParameters,
				vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(),
				m_inputLayout.GetAddressOf()));
		}
	}

	// コンスタント・テクスチャ・サンプラの作成
	D3D11_SHADER_BUFFER_DESC shaderBufferDesc = {};
	for (auto stage = core::ShaderStage::VS; stage < core::ShaderStage::MAX; ++stage)
	{
		const auto& stageIndex = static_cast<size_t>(stage);
		const auto& reflection = reflections[stageIndex];
		if (!reflection) continue;
		// シェーダ情報取得
		reflection->GetDesc(&shaderDesc);

		// リフレクションからコンスタントバッファ読み込み
		std::uint32_t slotOffset = 0;
		m_cbufferLayouts[stageIndex].reserve(shaderDesc.ConstantBuffers);
		for (std::uint32_t cbIdx = 0; cbIdx < shaderDesc.ConstantBuffers; ++cbIdx)
		{
			auto* constantBuffer = reflection->GetConstantBufferByIndex(cbIdx);
			constantBuffer->GetDesc(&shaderBufferDesc);

			// 共通の定数バッファは飛ばす??
			std::string cbName = shaderBufferDesc.Name;
			if (cbName == core::SHADER::SHADER_CB_NAME_SYSTEM ||
				cbName == core::SHADER::SHADER_CB_NAME_TRANSFORM ||
				cbName == core::SHADER::SHADER_CB_NAME_ANIMATION)
			{
				++slotOffset;
				continue;
			}

			// Gbufferシェーダーの場合
			if (cbName == core::SHADER::SHADER_CB_NAME_GBUFFER)
			{
				m_type = core::ShaderType::Deferred;
			}

			// レイアウト生成
			CBufferLayout cbLayout(cbIdx - slotOffset, shaderBufferDesc.Name, shaderBufferDesc.Size);
			cbLayout.variables.resize(shaderBufferDesc.Variables);

			// CB変数のレイアウト作成
			D3D11_SHADER_VARIABLE_DESC varDesc;
			for (std::uint32_t v = 0; v < shaderBufferDesc.Variables; ++v)
			{
				// 変数情報取得
				auto* variable = constantBuffer->GetVariableByIndex(v);
				variable->GetDesc(&varDesc);
				// 変数情報代入
				cbLayout.variables[v].name = varDesc.Name;
				cbLayout.variables[v].size = varDesc.Size;
				cbLayout.variables[v].offset = varDesc.StartOffset;
				cbLayout.variables[v].stage = stageIndex;
				cbLayout.variables[v].slot = cbIdx - slotOffset;
				// デフォルト値がある場合
				if (varDesc.DefaultValue != nullptr)
				{
					std::unique_ptr<std::byte[]> defaultValue = 
						std::make_unique<std::byte[]>(varDesc.Size);
					std::memcpy(defaultValue.get(), varDesc.DefaultValue, varDesc.Size);
					m_cbufferDefaults[varDesc.Name] = std::move(defaultValue);
				}
			}
			// コンスタントバッファレイアウト格納
			m_cbufferLayouts[stageIndex].emplace(cbIdx - slotOffset, cbLayout);
		}


		// テクスチャ・サンプラのリフレクション
		for (std::uint32_t i = 0; i < shaderDesc.BoundResources; ++i)
		{
			// バインド情報取得
			D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
			reflection->GetResourceBindingDesc(i, &shaderInputBindDesc);



			switch (shaderInputBindDesc.Type)
			{
			case D3D_SIT_TEXTURE:
				// 共通リソースはスキップ
				if (//shaderInputBindDesc.BindPoint == core::SHADER::SHADER_SRV_SLOT_MAINTEX		||
					shaderInputBindDesc.BindPoint == core::SHADER::SHADER_SRV_SLOT_SHADOWMAP ||
					shaderInputBindDesc.BindPoint == core::SHADER::SHADER_SRV_SLOT_SKYDOOM) continue;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;

			case D3D_SIT_SAMPLER:
				// 共通リソースはスキップ
				if (shaderInputBindDesc.BindPoint == core::SHADER::SHADER_SS_SLOT_MAIN		||
					shaderInputBindDesc.BindPoint == core::SHADER::SHADER_SS_SLOT_SHADOW ||
					shaderInputBindDesc.BindPoint == core::SHADER::SHADER_SS_SLOT_SKYBOX) continue;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;

			// 色々ある…
			case D3D_SIT_UAV_RWTYPED:
				break;
			case D3D_SIT_STRUCTURED:
				break;
			case D3D_SIT_TBUFFER:
				break;
			case D3D_SIT_CBUFFER:
				break;

			default:
				// エラーログ
				break;
			}
		}
	}
}

/// @brief シェーダオブジェクト生成
/// @param device デバイス
/// @param stage シェーダステージ
/// @param blob コンパイルデータ
void D3D11Shader::createShaderObjct(ID3D11Device1* device, const core::ShaderStage& stage, ComPtr<ID3DBlob>& blob)
{
	// com参照
	auto& shader = m_comShaders[static_cast<size_t>(stage)];

	// シェーダ種別生成
	switch (stage)
	{
	case core::ShaderStage::VS:
	{
		ID3D11VertexShader* d3d11Shader;
		CHECK_FAILED(device->CreateVertexShader(blob->GetBufferPointer(), 
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// インターフェース生成(参照カウント増加)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild), 
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// 参照カウントを減らす
		d3d11Shader->Release();
		vs = d3d11Shader;
	}
		break;
	case core::ShaderStage::GS:
	{
		ID3D11GeometryShader* d3d11Shader;
		CHECK_FAILED(device->CreateGeometryShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// インターフェース生成(参照カウント増加)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// 参照カウントを減らす
		d3d11Shader->Release();
		gs = d3d11Shader;
	}
		break;
	case core::ShaderStage::DS:
	{
		ID3D11DomainShader* d3d11Shader;
		CHECK_FAILED(device->CreateDomainShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// インターフェース生成(参照カウント増加)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// 参照カウントを減らす
		d3d11Shader->Release();
		ds = d3d11Shader;
	}
		break;
	case core::ShaderStage::HS:
	{
		ID3D11HullShader* d3d11Shader;
		CHECK_FAILED(device->CreateHullShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// インターフェース生成(参照カウント増加)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// 参照カウントを減らす
		d3d11Shader->Release();
		hs = d3d11Shader;
	}
		break;
	case core::ShaderStage::PS:
	{
		ID3D11PixelShader* d3d11Shader;
		CHECK_FAILED(device->CreatePixelShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// インターフェース生成(参照カウント増加)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// 参照カウントを減らす
		d3d11Shader->Release();
		ps = d3d11Shader;
	}
		break;
	case core::ShaderStage::CS:
	{
		ID3D11ComputeShader* d3d11Shader;
		CHECK_FAILED(device->CreateComputeShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// インターフェース生成(参照カウント増加)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// 参照カウントを減らす
		d3d11Shader->Release();
		cs = d3d11Shader;
	}
		break;
	}
}
