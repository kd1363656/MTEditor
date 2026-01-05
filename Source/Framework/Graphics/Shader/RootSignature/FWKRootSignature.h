#pragma once

namespace FWK::Graphics
{
	class RootSignature
	{
	public:

		enum class RangeType
		{
			CBV ,
			SRV ,
			UAV ,
		};

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

		void Create(GraphicsDevice* a_graphicsDevice , const std::vector<RangeType>& a_rangeTypeList);

		// ルートシグネチャの取得
		ID3D12RootSignature* GetRootSignature() { return m_rootSignature.Get(); }

	private:

		// レンジの作成
		void CreateRange(D3D12_DESCRIPTOR_RANGE& a_range , RangeType a_type , const int a_count);

		void CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC& a_samplerDesc , TextureAddressMode a_mode);

		FWK::Graphics::GraphicsDevice* m_device        = nullptr;
		ComPtr<ID3DBlob>               m_rootBlob      = nullptr;
		ComPtr<ID3D12RootSignature>    m_rootSignature = nullptr;
	};
}