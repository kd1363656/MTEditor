#pragma once

namespace FWK::Graphics
{
	class GraphicsDevice final : public FWK::SingletonBase<FWK::Graphics::GraphicsDevice>
	{
	public:

		bool Init(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size);

		// 画面(スワップチェイン)の切り替え
		void ScreenFlip();

		// コマンドキューの同期待ち
		void WaitForCommandQueue();

		ID3D12Device8* GetDevice() const { return m_device.Get(); }

		ID3D12GraphicsCommandList6* GetCmdList() const { return m_cmdList.Get(); }

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

		// スワップチェイン"RTV"の作製
		bool CreateSwapChainRTV();

		// "Fence"の作製
		bool CreateFence();

		// リソースとして引数に渡したバッファの扱いを変更する関数
		void SetResourceBarrier(ID3D12Resource* a_resource , D3D12_RESOURCE_STATES a_before , D3D12_RESOURCE_STATES a_after) const;

		// デバックレイヤーを適用
		void EnableDebugLayer() const;

		std::array<Microsoft::WRL::ComPtr<ID3D12Resource> , 2LLU> m_swapChainBuffers;

		FWK::ComPtr<ID3D12Device8> m_device      = nullptr;
		FWK::ComPtr<IDXGIFactory6> m_dxgiFactory = nullptr;

		FWK::ComPtr<ID3D12CommandAllocator>     m_cmdAllocator = nullptr;		// "GPU"に対する命令を保持するリスト
		FWK::ComPtr<ID3D12GraphicsCommandList6> m_cmdList      = nullptr;		// 命令のためのインターフェース、必ず"Close"命令を入れないと"GPU"は命令を実行できない
		FWK::ComPtr<ID3D12CommandQueue>         m_cmdQueue     = nullptr;		// コマンドリストを用いて描画などの命令を実行
		
		// ダブルバッファリングによるレンダーターゲットで画面のちらつきを無くすために必要
		FWK::ComPtr<IDXGISwapChain4> m_swapChain = nullptr; 

		// コマンドキューの実行同期をとるためのもの
		FWK::ComPtr<ID3D12Fence> m_fence = nullptr;

		std::unique_ptr<FWK::Graphics::RTVHeap> m_rtvHeap = nullptr;
		
		UINT64 m_fenceVal = 0ULL;

		// ===================
		// シングルトン
		// ===================
		friend class FWK::SingletonBase<FWK::Graphics::GraphicsDevice>;

		GraphicsDevice () = default;
		~GraphicsDevice() override
		{
			// コマンドキュー実行中にリソースが解放されることを防ぐ
			WaitForCommandQueue();
		}
	};
}