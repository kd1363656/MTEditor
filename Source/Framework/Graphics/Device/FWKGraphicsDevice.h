#pragma once

namespace FWK
{
	class GraphicsDevice final : public FWK::SingletonBase<FWK::GraphicsDevice>
	{
	public:

		bool Init(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size);

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

		// コマンドリストの作製
		bool CreateCommandList();

		// スワップチェインの作成
		bool CreateSwapChain(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size);

		Microsoft::WRL::ComPtr<ID3D12Device8>   m_device      = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory6>   m_dxgiFactory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain   = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>     m_cmdAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_cmdList      = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>         m_cmdQueue     = nullptr;

		// ===================
		// シングルトン
		// ===================
		friend class FWK::SingletonBase<FWK::GraphicsDevice>;

		GraphicsDevice ()          = default;
		~GraphicsDevice() override = default;
	};
}