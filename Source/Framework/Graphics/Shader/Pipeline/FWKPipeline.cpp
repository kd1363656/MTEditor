#include "FWKPipeline.h"

void FWK::Graphics::Pipeline::SetRenderSettings(FWK::Graphics::GraphicsDevice*                   a_graphicsDevice  , 
											    FWK::Graphics::RootSignature*                    a_rootSignature   , 
											    const std::vector<FWK::CommonEnum::InputLayout>& a_inputLayoutList , 
												FWK::CommonEnum::CullMode						 a_cullMode        , 
												FWK::CommonEnum::BlendMode						 a_blendMode	   , 
												FWK::CommonEnum::PrimitiveTopologyType			 a_topologyType)
{
	if (!a_graphicsDevice) { return; }

	m_device          = a_graphicsDevice;
	m_rootSignature   = a_rootSignature;
	m_inputLayoutList = a_inputLayoutList;
	m_cullMode        = a_cullMode;
	m_blendMode       = a_blendMode;
	m_topologyType    = a_topologyType;
}

void FWK::Graphics::Pipeline::Create(std::vector<ID3DBlob*>			a_blobList    , 
								     const std::vector<DXGI_FORMAT> a_formatList  , 
									 const bool						a_isDepth	  , 
									 const bool						a_isDepthMask , 
									 const int						a_rtvCount    , 
									 const bool						a_isWireFrame)
{
	if (!m_rootSignature)      { return; }
	if (!m_device)		       { return; }
	if (a_blobList.size() < 5) { return; }

	std::vector<D3D12_INPUT_ELEMENT_DESC> l_inputLayoutList;

	SetInputLayout(l_inputLayoutList , m_inputLayoutList);

	// "GraphicsPipelineState"の各種設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC l_graphicsPipelineState = {};

	// 頂点シェーダーをセット
	if (a_blobList[0]) 
	{
		l_graphicsPipelineState.VS.pShaderBytecode = a_blobList[0]->GetBufferPointer();
		l_graphicsPipelineState.VS.BytecodeLength  = a_blobList[0]->GetBufferSize   ();
	}

	// ハルシェーダーをセット
	if (a_blobList[1])
	{
		l_graphicsPipelineState.HS.pShaderBytecode = a_blobList[1]->GetBufferPointer();
		l_graphicsPipelineState.HS.BytecodeLength  = a_blobList[1]->GetBufferSize   ();
	}

	// ドメインシェーダーをセット
	if (a_blobList[2])
	{
		l_graphicsPipelineState.DS.pShaderBytecode = a_blobList[2]->GetBufferPointer();
		l_graphicsPipelineState.DS.BytecodeLength  = a_blobList[2]->GetBufferSize   ();
	}	

	// ジオメトリシェーダーをセット
	if (a_blobList[3])
	{
		l_graphicsPipelineState.GS.pShaderBytecode = a_blobList[3]->GetBufferPointer();
		l_graphicsPipelineState.GS.BytecodeLength  = a_blobList[3]->GetBufferSize   ();
	}
	// ピクセルシェーダーをセット
	if (a_blobList[4])
	{
		l_graphicsPipelineState.PS.pShaderBytecode = a_blobList[4]->GetBufferPointer();
		l_graphicsPipelineState.PS.BytecodeLength  = a_blobList[4]->GetBufferSize   ();
	}

	l_graphicsPipelineState.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// カリングモードをセット
	l_graphicsPipelineState.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(m_cullMode);

	// フィルターモードをセット
	if (a_isWireFrame)
	{
		l_graphicsPipelineState.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;	// 中身を塗りつぶさない
	}
	else
	{
		l_graphicsPipelineState.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;		// 中身を塗りつぶす
	}

	// 深度設定をセット
	if (a_isDepth)
	{
		l_graphicsPipelineState.RasterizerState.DepthClipEnable = true;
		l_graphicsPipelineState.DepthStencilState.DepthEnable   = true;
		l_graphicsPipelineState.DepthStencilState.StencilEnable = false;

		if (a_isDepthMask)
		{
			l_graphicsPipelineState.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			l_graphicsPipelineState.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}

		l_graphicsPipelineState.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		l_graphicsPipelineState.DSVFormat                   = DXGI_FORMAT_D32_FLOAT;
	}
	else
	{
		l_graphicsPipelineState.RasterizerState.DepthClipEnable = false;
		l_graphicsPipelineState.DepthStencilState.DepthEnable   = false;
		l_graphicsPipelineState.DepthStencilState.StencilEnable = false;
	}

	l_graphicsPipelineState.BlendState.AlphaToCoverageEnable  = false;
	l_graphicsPipelineState.BlendState.IndependentBlendEnable = false;

	// ブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC l_blendDesc = {};
	SetBlendMode(l_blendDesc , m_blendMode);

	l_graphicsPipelineState.BlendState.RenderTarget[0]     = l_blendDesc;
	l_graphicsPipelineState.InputLayout.pInputElementDescs = l_inputLayoutList.data      ();
	l_graphicsPipelineState.InputLayout.NumElements        = (int)(m_inputLayoutList.size());
	l_graphicsPipelineState.PrimitiveTopologyType		   = (a_blobList[3] && a_blobList[4]) ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH : static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_topologyType);
	
	// "RTV"をセット
	l_graphicsPipelineState.NumRenderTargets = a_rtvCount;

	// "RTV"フォーマットのセット
	for (int l_i = 0; l_i < a_rtvCount; ++l_i)
	{
		l_graphicsPipelineState.RTVFormats[l_i] = a_formatList[l_i];
	}

	l_graphicsPipelineState.SampleDesc.Count = 1;									// サンプリングは"1"ピクセル付き
	l_graphicsPipelineState.pRootSignature   = m_rootSignature->GetRootSignature();

	auto l_device = m_device->GetDevice();
	if (!l_device) { return; }

	auto l_hr = l_device->CreateGraphicsPipelineState(&l_graphicsPipelineState , IID_PPV_ARGS(&m_pipelineState));

	if (FAILED(l_hr))
	{
		assert(false && "パイプラインステートの作成に失敗しました。");
		return;
	}
}

void FWK::Graphics::Pipeline::SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& a_inputElementList , const std::vector<FWK::CommonEnum::InputLayout>& a_inputLayoutList)
{
	for (int l_i = 0; l_i < (int)a_inputLayoutList.size(); ++l_i)
	{
		if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::POSITION)
		{
			a_inputElementList.emplace_back( D3D12_INPUT_ELEMENT_DESC 
			{ 
			   "POSITION"                                 , 
			   0                                          , 
			   DXGI_FORMAT_R32G32B32_FLOAT                , 
			   0						                  , 
			   D3D12_APPEND_ALIGNED_ELEMENT               , 
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 
			   0 
			});
		}
		else if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::TEXCOORD)
		{
			a_inputElementList.emplace_back( D3D12_INPUT_ELEMENT_DESC 
			{ 
			   "TEXCOORD"                                 , 
			   0                                          , 
			   DXGI_FORMAT_R32G32_FLOAT                   , 
			   0						                  , 
			   D3D12_APPEND_ALIGNED_ELEMENT               , 
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 
			   0 
			});
		}
		else if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::NORMAL)
		{
			a_inputElementList.emplace_back( D3D12_INPUT_ELEMENT_DESC 
			{ 
			   "NORMAL"                                   , 
			   0                                          , 
			   DXGI_FORMAT_R32G32B32_FLOAT                , 
			   0						                  , 
			   D3D12_APPEND_ALIGNED_ELEMENT               , 
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 
			   0 
			});
		}
		else if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::COLOR)
		{
			a_inputElementList.emplace_back(D3D12_INPUT_ELEMENT_DESC
			{
			   "COLOR"                                    ,
			   0                                          ,
			   DXGI_FORMAT_R8G8B8A8_UNORM                 ,
			   0						                  ,
			   D3D12_APPEND_ALIGNED_ELEMENT               ,
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,
			   0
			});
		}
		else if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::TANGENT)
		{
			a_inputElementList.emplace_back(D3D12_INPUT_ELEMENT_DESC
				{
				   "TANGENT"                                  ,
				   0                                          ,
				   DXGI_FORMAT_R32G32B32_FLOAT                ,
				   0						                  ,
				   D3D12_APPEND_ALIGNED_ELEMENT               ,
				   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,
				   0
				});
		}
		else if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::SKININDEX)
		{
			a_inputElementList.emplace_back(D3D12_INPUT_ELEMENT_DESC
			{
			   "SKININDEX"                                ,
			   0                                          ,
			   DXGI_FORMAT_R16G16B16A16_FLOAT             ,
			   0						                  ,
			   D3D12_APPEND_ALIGNED_ELEMENT               ,
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,
			   0
			});
		}
		else if (a_inputLayoutList[l_i] == FWK::CommonEnum::InputLayout::SKINWEIGHT)
		{
			a_inputElementList.emplace_back(D3D12_INPUT_ELEMENT_DESC
			{
			   "SKINWEIGHT"                                 ,
			   0                                          ,
			   DXGI_FORMAT_R32G32B32A32_FLOAT             ,
			   0						                  ,
			   D3D12_APPEND_ALIGNED_ELEMENT               ,
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,
			   0
			});
		}
	}
}

void FWK::Graphics::Pipeline::SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& a_blendDesc , FWK::CommonEnum::BlendMode a_blendMode)
{
	a_blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	a_blendDesc.BlendEnable			  = true;

	switch (a_blendMode)
	{
		// 加算合成
		case FWK::CommonEnum::BlendMode::Add:
			a_blendDesc.BlendOp   = D3D12_BLEND_OP_ADD;
			a_blendDesc.SrcBlend  = D3D12_BLEND_SRC_ALPHA;
			a_blendDesc.DestBlend = D3D12_BLEND_ONE;

			a_blendDesc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
			a_blendDesc.SrcBlendAlpha  = D3D12_BLEND_ONE;
			a_blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			a_blendDesc.LogicOp        = D3D12_LOGIC_OP_NOOP;
		break;

		// 半透明
		case FWK::CommonEnum::BlendMode::Alpha:
			a_blendDesc.BlendOp   = D3D12_BLEND_OP_ADD;
			a_blendDesc.SrcBlend  = D3D12_BLEND_SRC_ALPHA;
			a_blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

			a_blendDesc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
			a_blendDesc.SrcBlendAlpha  = D3D12_BLEND_ONE;
			a_blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			a_blendDesc.LogicOp        = D3D12_LOGIC_OP_NOOP;
		break;

		default:
		break;
	}
}