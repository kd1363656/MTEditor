#pragma once

namespace FWK::Graphics
{
	class Pipeline final
	{
	public:

		Pipeline () = default;
		~Pipeline() = default;

		// 描画設定のセット
		void SetRenderSettings(FWK::Graphics::GraphicsDevice*                   a_graphicsDevice  , 
							   FWK::Graphics::RootSignature*                    a_rootSignature   , 
							   const std::vector<FWK::CommonEnum::InputLayout>& a_inputLayoutList , 
							   FWK::CommonEnum::CullMode						    a_cullMode        , 
							   FWK::CommonEnum::BlendMode					    a_blendMode       , 
							   FWK::CommonEnum::PrimitiveTopologyType		    a_topologyType);

		void Create(std::vector<ID3DBlob*>		   a_blobList    , 
					const std::vector<DXGI_FORMAT> a_formatList  , 
					const bool					   a_isDepth     , 
					const bool					   a_isDepthMask , 
					const int					   a_rtvCount    , 
					const bool					   a_isWireFrame);

		// パイプライの取得
		ID3D12PipelineState* GetPipeline() const { return m_pipelineState.Get(); }

		// トポロジータイプの取得
		FWK::CommonEnum::PrimitiveTopologyType GetTopologyType() { return m_topologyType; }

	private:

		void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& a_inputElementList , const std::vector<FWK::CommonEnum::InputLayout>& a_inputLayoutList);

		// ブレンドモードのセット
		void SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& a_blendDesc , FWK::CommonEnum::BlendMode a_blendMode);

		GraphicsDevice*				   m_device        = nullptr;
		FWK::Graphics::RootSignature*  m_rootSignature = nullptr;

		std::vector<FWK::CommonEnum::InputLayout> m_inputLayoutList;
		FWK::CommonEnum::CullMode                 m_cullMode      = FWK::CommonEnum::CullMode::None;
		FWK::CommonEnum::BlendMode				  m_blendMode     = FWK::CommonEnum::BlendMode::Add;
		FWK::CommonEnum::PrimitiveTopologyType    m_topologyType  = FWK::CommonEnum::PrimitiveTopologyType::Undefined;

		ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;
	};
}