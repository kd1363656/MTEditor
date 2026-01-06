#include "FWKShader.h"

void FWK::Graphics::Shader::Create(FWK::Graphics::GraphicsDevice*                a_graphicsDevice   , 
								   const std::wstring&			                 a_filePath         , 
								   const RenderingSetting&		                 a_renderingSetting , 
								   const std::vector<FWK::CommonEnum::RangeType> a_rangeTypeList)
{
	m_device = a_graphicsDevice;

	LoadShaderFile(a_filePath);

	if (!m_upRootSignature)
	{
		m_upRootSignature = std::make_unique<FWK::Graphics::RootSignature>();
	}

	m_upRootSignature->Create(a_graphicsDevice , a_rangeTypeList);

	if (!m_upPipeline)
	{
		m_upPipeline = std::make_unique<FWK::Graphics::Pipeline>();
	}

	m_upPipeline->SetRenderSettings(a_graphicsDevice                        , 
									m_upRootSignature.get()                 , 
								    a_renderingSetting.inputLayoutList      , 
								    a_renderingSetting.cullMode		        , 
									a_renderingSetting.blendMode	        , 
									a_renderingSetting.primitiveTopologyType);

	m_upPipeline->Create({ m_vsBlob , m_hsBlob , m_gsBlob , } , 
						  a_renderingSetting.formatList       , 
						  a_renderingSetting.isDepth          , 
						  a_renderingSetting.isDepthMask      , 
						  a_renderingSetting.rtvCount		  , 
						  a_renderingSetting.isWireFrame);
}

void FWK::Graphics::Shader::Begin(const int a_w, const int a_h)
{
	if (!m_device) { return; }

	auto* l_cmdList = m_device->GetCmdList();
	if (!l_cmdList) { return; }

	l_cmdList->SetPipelineState(m_upPipeline->GetPipeline());

	// ルートシグネチャのセット
	l_cmdList->SetGraphicsRootSignature(m_upRootSignature->GetRootSignature());

	D3D12_PRIMITIVE_TOPOLOGY_TYPE l_topologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_upPipeline->GetTopologyType());

	switch (l_topologyType)
	{
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
			m_device->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;

		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
			m_device->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;

		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
			m_device->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;

		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
			m_device->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		break;

		default:
		break;
	}

	D3D12_VIEWPORT l_viewPort = {};
	D3D12_RECT	   l_rect     = {};

	l_viewPort.Width  = static_cast<float>(a_w);
	l_viewPort.Height = static_cast<float>(a_h);

	l_rect.right  = a_w;
	l_rect.bottom = a_h;

	FWK::Graphics::GraphicsDevice::GetInstance().GetCmdList()->RSSetViewports   (1 , &l_viewPort);
	FWK::Graphics::GraphicsDevice::GetInstance().GetCmdList()->RSSetScissorRects(1 , &l_rect);
}

void FWK::Graphics::Shader::DrawMesh(const FWK::Graphics::Mesh& a_mesh)
{
	a_mesh.DrawInstanced();
}

void FWK::Graphics::Shader::LoadShaderFile(const std::wstring& a_filePath)
{
	ID3DInclude* l_include   = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	UINT         l_flag      = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ID3DBlob*    l_errorBlob = nullptr;

	std::wstring l_format      = L".hlsl";
	std::wstring l_currentPath = L"Asset/Shader/";

	// 頂点シェーダーのコンパイル
	{
		std::wstring l_fullFilePath = l_currentPath + a_filePath + L"_VS" + l_format;

		auto l_hr = D3DCompileFromFile(l_currentPath.c_str() , 
									   nullptr               , 
									   l_include             , 
									   "main"                , 
									   "vs_5_0"              , 
									    l_flag               ,
										0                    , 
										&m_vsBlob            , 
										&l_errorBlob);

		if (FAILED(l_hr))
		{
			assert(false && "頂点シェーダーのコンパイルに失敗しました。");
			return;
		}
	}

	// ハルシェーダーのコンパイル
	{
		std::wstring l_fullFilePath = l_currentPath + a_filePath + L"_HS" + l_format;
		auto l_hr = D3DCompileFromFile(l_fullFilePath.c_str() , 
									   nullptr                , 
									   l_include              , 
									   "main"				  , 
									   "hs_5_0"				  , 
									   l_flag				  , 
									   0					  , 
									   &m_hsBlob			  , 
									   &l_errorBlob);
		// ハルシェーダーはなくてもいい
	}

	// ドメインシェーダーのコンパイル
	{
		std::wstring l_fullFilePath = l_currentPath + a_filePath + L"_DS" + l_format;
		auto l_hr = D3DCompileFromFile(l_fullFilePath.c_str() , 
									   nullptr                , 
									   l_include              , 
									   "main"				  , 
									   "ds_5_0"				  , 
									   l_flag				  , 
									   0					  , 
									   &m_dsBlob			  , 
									   &l_errorBlob);

		// ドメインシェーダーはなくてもいい
	}

	// ジオメトリシェーダーのコンパイル
	{
		std::wstring l_fullFilePath = l_currentPath + a_filePath + L"_GS" + l_format;
		auto l_hr = D3DCompileFromFile(l_fullFilePath.c_str() , 
									   nullptr                , 
									   l_include              , 
									   "main"				  , 
									   "gs_5_0"				  , 
									   l_flag				  , 
									   0					  , 
									   &m_dsBlob			  , 
									   &l_errorBlob);

		// ジオメトリシェーダーはなくてもいい
	}

	// ピクセルシェーダーのコンパイル
	{
		std::wstring l_fullFilePath = l_currentPath + a_filePath + L"_PS" + l_format;
		auto l_hr = D3DCompileFromFile(l_fullFilePath.c_str() , 
									   nullptr                , 
									   l_include              , 
									   "main"				  , 
									   "ps_5_0"				  , 
									   l_flag				  , 
									   0					  , 
									   &m_psBlob			  , 
									   &l_errorBlob);

		if (FAILED(l_hr))
		{
			assert(false && "ピクセルシェーダーのコンパイルに失敗しました。");
			return;
		}
	}
}
