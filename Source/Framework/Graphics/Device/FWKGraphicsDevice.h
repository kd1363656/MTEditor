#pragma once

namespace FWK
{
	class GraphicsDevice final : public FWK::SingletonBase<FWK::GraphicsDevice>
	{
	public:

		bool Init();

	private:

		enum class GPUTier
		{ 
			NVIDIA   ,
			Amd      ,
			Intel    ,
			Arm      ,
			Qualcomm ,
			Kind     ,
		};

		// ファクトリーの作成
		bool CreateFactory();

		// デバイスの作成
		bool CreateDevice();

		Microsoft::WRL::ComPtr<ID3D12Device8>   m_device      = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory6>   m_dxgiFactory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain   = nullptr;

		// ===================
		// シングルトン
		// ===================
		friend class FWK::SingletonBase<FWK::GraphicsDevice>;

		GraphicsDevice ()          = default;
		~GraphicsDevice() override = default;
	};
}