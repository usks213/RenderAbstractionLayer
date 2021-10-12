/*****************************************************************//**
 * \file   D3D12_Material.h
 * \brief  DirectX12マテリアル
 *
 * \author USAMI KOSHI
 * \date   2021/10/11
 *********************************************************************/

#include "D3D12_Material.h"
#include "D3D12_Shader.h"
using namespace d3d12;


namespace
{
	constexpr D3D12_SHADER_VISIBILITY SHADER_VISIBILITYS[static_cast<size_t>(core::ShaderStage::CS)] = {
		D3D12_SHADER_VISIBILITY_VERTEX,
		D3D12_SHADER_VISIBILITY_HULL,
		D3D12_SHADER_VISIBILITY_DOMAIN,
		D3D12_SHADER_VISIBILITY_GEOMETRY,
		D3D12_SHADER_VISIBILITY_PIXEL,
	};
}

/// @brief コンストラクタ
/// @param device デバイス
/// @param id マテリアルID
/// @param name マテリアル名
/// @param shader シェーダ
D3D12Material::D3D12Material(ID3D12Device* device, const core::MaterialID& id,
	const std::string& name, core::CoreShader& shader) :
	core::CoreMaterial(id,name,shader)
{
	// 全ディスクリプタレンジ・パラメータ
	std::vector<D3D12_DESCRIPTOR_RANGE> aRanges;
	std::vector<D3D12_ROOT_PARAMETER> aParameters;

	// コンスタントバッファの確保(GPU)
	for (core::ShaderStage stage = core::ShaderStage::VS; stage < core::ShaderStage::CS; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);

		// ヒープの作成
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// ピープタイプ
		heapDesc.NumDescriptors = m_cbufferData[stageIndex].size();					// バッファ数
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーリソースか
		heapDesc.NodeMask = 0;	// 今は0
		CHECK_FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_pCBufferHeap[stageIndex].GetAddressOf())));

		// ヒーププロパテェの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;			// ピープのタイプ
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// ページング設定
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;		// メモリプール設定
		prop.CreationNodeMask = 1;								// 今は1
		prop.VisibleNodeMask = 1;								// 今は1

		// リソースの設定
		D3D12_RESOURCE_DESC d3dDesc = {};
		d3dDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの種別
		d3dDesc.Alignment = 0;								// 配置指定？
		d3dDesc.Width = 0;								// 後で個別指定
		d3dDesc.Height = 1;								// 高さは１
		d3dDesc.DepthOrArraySize = 1;								// 3Dはリソースの深さ、1D,2Dの配列は配列サイズ
		d3dDesc.MipLevels = 1;								// バッファは１
		d3dDesc.Format = DXGI_FORMAT_UNKNOWN;				// バッファはUNKNOWN
		d3dDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };			// バッファはデフォルト
		d3dDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// とりあえずこれ！
		d3dDesc.Flags = D3D12_RESOURCE_FLAG_NONE;			// CBufferはなし

		// ヒープハンドル
		auto handle = m_pCBufferHeap[stageIndex]->GetCPUDescriptorHandleForHeapStart();
		auto cbufferIncreSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//--- CBuffer確保
		for (auto& cbData : m_cbufferData[stageIndex])
		{
			// CBufferサイズ
			d3dDesc.Width	= (cbData.second.size % 256 + 1) * 256;
			// リソース生成
			CHECK_FAILED(device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&d3dDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_d3dCbuffer[stageIndex][cbData.first].GetAddressOf())
			));
			// 初期のデータ格納なし
			cbData.second.isUpdate = true;

			// 定数バッファビューの作成
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbufferDesc = {};
			cbufferDesc.BufferLocation = m_d3dCbuffer[stageIndex][cbData.first]->GetGPUVirtualAddress();
			cbufferDesc.SizeInBytes = (cbData.second.size % 256 + 1) * 256;
			device->CreateConstantBufferView(&cbufferDesc, handle);

			// ハンドルポインタ加算
			handle.ptr += cbufferIncreSize;

		}

		//--- ディスクリプタレンジ・パラメータ ---

		// 定数バッファ
		if (m_cbufferData[stageIndex].size() > 0)
		{
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range.NumDescriptors = m_cbufferCount;
			range.BaseShaderRegister = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			aRanges.push_back(range);

			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.DescriptorTable.pDescriptorRanges = &aRanges.back();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// テクスチャ
		for (auto& tex : m_textureData[stageIndex])
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			param.Descriptor.ShaderRegister = tex.first;
			param.Descriptor.RegisterSpace = 0;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// サンプラー
		if (m_samplerData[stageIndex].size() > 0)
		{

		}
	}

	// 固定CBufferの格納
	D3D12_ROOT_PARAMETER transParam = {};
	transParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	transParam.Descriptor.ShaderRegister = core::SHADER::SHADER_CB_SLOT_TRANSFORM;
	transParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	aParameters.push_back(transParam);

	D3D12_ROOT_PARAMETER systemParam = {};
	systemParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	systemParam.Descriptor.ShaderRegister = core::SHADER::SHADER_CB_SLOT_SYSTEM;
	systemParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	aParameters.push_back(systemParam);

	// ルートシグネチャーの生成
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = aParameters.size();
	rootSignatureDesc.pParameters = aParameters.data();
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	if (rootSigBlob)
	{
		device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(m_pRootSignature.GetAddressOf()));
	}
	else
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		OutputDebugStringA(errstr.c_str());
	}
	rootSigBlob->Release();

	d3d12::D3D12Shader& shader12 = *static_cast<D3D12Shader*>(&shader);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = nullptr;
	gpipeline.VS.pShaderBytecode = shader12.m_pShaderBlob[0]->GetBufferPointer();
	gpipeline.VS.BytecodeLength = shader12.m_pShaderBlob[0]->GetBufferSize();
	gpipeline.PS.pShaderBytecode = shader12.m_pShaderBlob[4]->GetBufferPointer();
	gpipeline.PS.BytecodeLength = shader12.m_pShaderBlob[4]->GetBufferSize();

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//中身は0xffffffff

	//
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};

	//ひとまず加算や乗算やαブレンディングは使用しない
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//ひとまず論理演算は使用しない
	renderTargetBlendDesc.LogicOpEnable = false;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	gpipeline.RasterizerState.MultisampleEnable = false;//まだアンチェリは使わない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//中身を塗りつぶす
	gpipeline.RasterizerState.DepthClipEnable = true;//深度方向のクリッピングは有効に

	//残り
	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DepthStencilState.StencilEnable = false;

	gpipeline.InputLayout.pInputElementDescs = shader12.m_inputElementDesc.data();//レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = shader12.m_inputElementDesc.size();//レイアウト配列数

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//ストリップ時のカットなし
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

	gpipeline.NumRenderTargets = 1;//今は１つのみ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～1に正規化されたRGBA

	gpipeline.SampleDesc.Count = 1;//サンプリングは1ピクセルにつき１
	gpipeline.SampleDesc.Quality = 0;//クオリティは最低

	gpipeline.pRootSignature = m_pRootSignature.Get();
	ID3D12PipelineState* _pipelinestate = nullptr;
	device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));


}
