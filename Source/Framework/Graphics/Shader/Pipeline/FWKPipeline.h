#pragma once

namespace FWK::Graphics
{
	class Pipeline
	{
	public:

		enum class CullMode
		{
			None  = D3D12_CULL_MODE_NONE  ,
			Front = D3D12_CULL_MODE_FRONT ,
			Back  = D3D12_CULL_MODE_BACK,
		};

		enum class BlendMode
		{
			Add   , 
			Alpha ,
		};

		enum class InputLayout
		{
			POSITION   ,
			TEXCOORD   ,
			NORMAL     ,
			TANGENT    ,
			COLOR      ,
			SKININDEX  ,
			SKINWEIGHT ,
		};

		enum class PrimitiveTopologyType
		{
			Undefined = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED ,
			Point     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT     ,
			Line      = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE      ,
			Triangle  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE  ,
			Patch     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
		};

	public:

		Pipeline () = default;
		~Pipeline() = default;

		// 描画設定のセット
		void SetREnderSettings(FWK::Graphics::GraphicsDevice*  a_graphicsDevice  , 
							   FWK::Graphics::RootSignature*   a_rootSignature   , 
							   const std::vector<InputLayout>& a_inputLayoutList , 
							   CullMode						   a_cullMode        , 
							   BlendMode					   a_blendMode       , 
							   PrimitiveTopologyType		   a_topologyType);

		void Create(std::vector<ID3DBlob*>		   a_blobList    , 
					const std::vector<DXGI_FORMAT> a_formatList  , 
					const bool					   a_isDepth     , 
					const bool					   a_isDepthMask , 
					const int					   a_rtvCount    , 
					const bool					   a_isWireFrame);

		// パイプライの取得
		ID3D12PipelineState* GetPipeline() const { return m_pipelineState.Get(); }

	private:

		void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& a_inputElementList , const std::vector<InputLayout>& a_inputLayoutList);

		// ブレンドモードのセット
		void SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& a_blendDesc , BlendMode a_blendMode);

		GraphicsDevice*				   m_device        = nullptr;
		FWK::Graphics::RootSignature*  m_rootSignature = nullptr;

		std::vector<InputLayout> m_inputLayoutList;
		CullMode                 m_cullMode;
		BlendMode				 m_blendMode;
		PrimitiveTopologyType    m_topologyType;

		ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;
	};
}