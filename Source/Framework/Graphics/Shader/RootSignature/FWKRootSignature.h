#pragma once

namespace FWK::Graphics
{
	class RootSignature
	{
	public:

		enum class TextureAddressMode
		{
			Wrap  ,
			Clamp ,
		};

		enum class D3D12Filter
		{
			Point  ,
			Linear ,
		};

	public:

		RootSignature () = default;
		~RootSignature() = default;

		void Create(FWK::Graphics::GraphicsDevice* a_graphicsDevice , const std::vector<FWK::CommonEnum::RangeType>& a_rangeTypeList);

		// ルートシグネチャの取得
		ID3D12RootSignature* GetRootSignature() { return m_rootSignature.Get(); }

	private:

		// レンジの作成
		void CreateRange(D3D12_DESCRIPTOR_RANGE& a_range , const FWK::CommonEnum::RangeType a_type , const int a_count) const;

		void CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC&								a_samplerDesc , 
								 const FWK::Graphics::RootSignature::TextureAddressMode a_mode        , 
								 const FWK::Graphics::RootSignature::D3D12Filter        a_filter	      , 
								 const int												a_count) const;

		FWK::Graphics::GraphicsDevice* m_device        = nullptr;
		ComPtr<ID3DBlob>               m_rootBlob      = nullptr;
		ComPtr<ID3D12RootSignature>    m_rootSignature = nullptr;
	};
}