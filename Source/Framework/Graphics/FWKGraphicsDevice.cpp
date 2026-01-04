#include "FWKGraphicsDevice.h"

bool FWK::Graphics::GraphicsDevice::Init(const HWND a_hWND, const FWK::CommonStruct::Dimension2D& a_size)
{
	if (!CreateFactory())
	{
		assert(false && "ファクトリー作成失敗");
		return false;
	}

	if (!CreateDevice())
	{
		assert(false && "\"D3D12\"デバイス作成失敗");
		return false;
	}

	if (!CreateCommandList())
	{
		assert(false && "コマンドリストの作成失敗");
		return false;
	}

	if (!CreateSwapChain(a_hWND , a_size))
	{
		assert(false && "スワップチェインの作成失敗");
		return false;
	}

	if (!m_rtvHeap)
	{
		m_rtvHeap = std::make_unique<FWK::Graphics::RTVHeap>();
	}

	// レンダーターゲットン確保個数は"100"個だが超えてしまうとアサートでエラーが出る
	if (!m_rtvHeap->Create(m_device.Get() , 100))
	{
		assert(false && "RTVヒープの作成失敗");
		return false;
	}

	if (!CreateSwapChainRTV())
	{
		assert(false && "スワップチェイン\"RTV\"の作成失敗");
		return false;
	}


	return true;
}

bool FWK::Graphics::GraphicsDevice::CreateFactory()
{
	UINT l_flagsDXGI = 0U;

	l_flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;

	auto l_result = CreateDXGIFactory2(l_flagsDXGI , IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));

	if (FAILED(l_result))
	{
		return false;
	}

	return true;
}

bool FWK::Graphics::GraphicsDevice::CreateDevice()
{
	Microsoft::WRL::ComPtr<IDXGIAdapter>              l_selectAdapter = nullptr;
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>> l_adapters;
	std::vector<DXGI_ADAPTER_DESC>					  l_descs;

	// 使用中"PC"にある"GPU"ドライバーを検索して、あれば格納する
	for (UINT l_index = 0U; true; ++l_index)
	{
		l_adapters.emplace_back(nullptr);
		HRESULT l_result = m_dxgiFactory->EnumAdapters(l_index , &l_adapters[l_index]);

		if (l_result == DXGI_ERROR_NOT_FOUND) { break; }

		l_descs.push_back({});
		l_adapters[l_index]->GetDesc(&l_descs[l_index]);
	}

	GPUTier l_gpuTier = GPUTier::Kind;

	// 優先度の高い"GPU"ドライバーを使用する
	for (std::size_t l_i = 0; l_i < l_descs.size(); ++l_i)
	{
		if (std::wstring(l_descs[l_i].Description).find(L"NVIDIA") != std::wstring::npos)
		{
			l_selectAdapter = l_adapters[l_i];
			break;
		}
		else if (std::wstring(l_descs[l_i].Description).find(L"Amd") != std::wstring::npos)
		{
			if (l_gpuTier > GPUTier::Amd)
			{
				l_selectAdapter = l_adapters[l_i];
				l_gpuTier       = GPUTier::Amd;
			}
		}
		else if (std::wstring(l_descs[l_i].Description).find(L"Intel") != std::wstring::npos)
		{
			if (l_gpuTier > GPUTier::Intel)
			{
				l_selectAdapter = l_adapters[l_i];
				l_gpuTier       = GPUTier::Intel;
			}
		}
		else if (std::wstring(l_descs[l_i].Description).find(L"Arm") != std::wstring::npos)
		{
			if (l_gpuTier > GPUTier::Arm)
			{
				l_selectAdapter = l_adapters[l_i];
				l_gpuTier       = GPUTier::Arm;
			}
		}
		else if (std::wstring(l_descs[l_i].Description).find(L"Qualcomm") != std::wstring::npos)
		{
			if (l_gpuTier > GPUTier::Qualcomm)
			{
				l_selectAdapter = l_adapters[l_i];
				l_gpuTier       = GPUTier::Qualcomm;
			}
		}
	}

	D3D_FEATURE_LEVEL l_levels[] =
	{
		D3D_FEATURE_LEVEL_12_1 ,
		D3D_FEATURE_LEVEL_12_0 ,
		D3D_FEATURE_LEVEL_11_1 ,
		D3D_FEATURE_LEVEL_11_0 ,
	};

	// "Direct3D"デバイスの初期化
	D3D_FEATURE_LEVEL l_featureLevel;
	for (auto& l_lv : l_levels)
	{
		if (D3D12CreateDevice(l_selectAdapter.Get() , l_lv , IID_PPV_ARGS(&m_device)) == S_OK)
		{
			l_featureLevel = l_lv;
			break;				   // 生産可能なバージョンが見つかったらループ打ち切り
		}
	}

	return true;
}

bool FWK::Graphics::GraphicsDevice::CreateCommandList()
{
	if (!m_device)
	{
		return false;
	}

	auto l_hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT , IID_PPV_ARGS(&m_cmdAllocator));

	if (FAILED(l_hr))
	{
		return false;
	}

	l_hr = m_device->CreateCommandList(0							  , 
									   D3D12_COMMAND_LIST_TYPE_DIRECT ,
									   m_cmdAllocator.Get()			  , 
									   nullptr						  , 
									   IID_PPV_ARGS(&m_cmdList));

	if (FAILED(l_hr))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC l_cmdQueueDesc = {};
	l_cmdQueueDesc.Flags				    = D3D12_COMMAND_QUEUE_FLAG_NONE;	   // タイムアウトなし
	l_cmdQueueDesc.NodeMask                 = 0;								   // アダプターを一つしか使わない時は"0"でいい
	l_cmdQueueDesc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // プライオリティは特に指定なし
	l_cmdQueueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;	   // コマンドリストと合わせる

	// キュー作成
	l_hr = m_device->CreateCommandQueue(&l_cmdQueueDesc , IID_PPV_ARGS(&m_cmdQueue));

	if (FAILED(l_hr))
	{
		return false;
	}

	return true;
}

bool FWK::Graphics::GraphicsDevice::CreateSwapChain(const HWND a_hWND, const FWK::CommonStruct::Dimension2D& a_size)
{
	if (!m_dxgiFactory) { return false; }

	DXGI_SWAP_CHAIN_DESC1 l_swapChainDesc = {};
	l_swapChainDesc.Width                 = a_size.width;
	l_swapChainDesc.Height                = a_size.height;
	l_swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
	l_swapChainDesc.SampleDesc.Count      = 1;
	l_swapChainDesc.BufferUsage           = DXGI_USAGE_BACK_BUFFER;
	l_swapChainDesc.BufferCount           = 2;
	l_swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;			// フリップ後は速やかに破棄
	l_swapChainDesc.Flags				  = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ウィンドウとフルスクリーン切り替え可能

	auto l_result = m_dxgiFactory->CreateSwapChainForHwnd(m_cmdQueue.Get() , 
														  a_hWND           , 
													      &l_swapChainDesc , 
														  nullptr          ,
														  nullptr          ,
														  (IDXGISwapChain1**) m_swapChain.ReleaseAndGetAddressOf());

	if (FAILED(l_result))
	{
		return false;
	}

	return true;
}

bool FWK::Graphics::GraphicsDevice::CreateSwapChainRTV()
{
	for (int l_i = 0; l_i < (int)m_swapChainBuffers.size(); ++l_i)
	{
		auto l_hr = m_swapChain->GetBuffer(l_i , IID_PPV_ARGS(&m_swapChainBuffers[l_i]));

		if (FAILED(l_hr))
		{
			return false;
		}

		m_rtvHeap->CreateRTV(m_swapChainBuffers[l_i].Get());
	}

	return true;
}