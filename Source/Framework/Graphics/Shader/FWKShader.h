#pragma once

namespace FWK::Graphics
{
	class Shader final
	{
	public:

		struct RenderingSetting
		{
			std::vector<FWK::CommonEnum::InputLayout> inputLayoutList;
			std::vector<DXGI_FORMAT>				  formatList;
			FWK::CommonEnum::CullMode				  cullMode              = FWK::CommonEnum::CullMode::Back;
			FWK::CommonEnum::BlendMode				  blendMode		        = FWK::CommonEnum::BlendMode::Alpha;
			FWK::CommonEnum::PrimitiveTopologyType    primitiveTopologyType = FWK::CommonEnum::PrimitiveTopologyType::Triangle;
			int										  rtvCount			    = 1;
			bool								      isDepth			    = true;
			bool									  isDepthMask		    = true;
			bool									  isWireFrame		    = false;
		};

	public:

		Shader () = default;
		~Shader() = default;

		void Create(FWK::Graphics::GraphicsDevice* a_graphicsDevice , const std::wstring& a_filePath , const RenderingSetting& a_renderingSetting , const std::vector<FWK::CommonEnum::RangeType> a_rangeTypeList);

		// 描画開始
		void Begin(const int a_w , const int a_h);
		
		// メッシュの描画
		void DrawMesh(const FWK::Graphics::Mesh& a_mesh);

	private:

		// シェーダーファイルのロード
		void LoadShaderFile(const std::wstring& a_filePath);

		FWK::Graphics::GraphicsDevice* m_device = nullptr;

		std::unique_ptr<FWK::Graphics::Pipeline>      m_upPipeline      = nullptr;
		std::unique_ptr<FWK::Graphics::RootSignature> m_upRootSignature = nullptr;

		ID3DBlob* m_vsBlob = nullptr;	// 頂点シェーダー
		ID3DBlob* m_hsBlob = nullptr;	// ハルシェーダー
		ID3DBlob* m_dsBlob = nullptr;	// ドメインシェーダー
		ID3DBlob* m_gsBlob = nullptr;	// ジオメトリシェーダー
		ID3DBlob* m_psBlob = nullptr;	// ピクセルシェーダー
	};
}