#include "FWKGraphicsDevice.h"

bool FWK::GraphicsDevice::Init()
{
	if (!CreateFactory())
	{
		assert(false && "ファクトリー作成失敗");
		return false;
	}

	if (!CreateFactory())
	{
		assert(false && "\"D3D12\"デバイス作成失敗");
		return false;
	}

	return true;
}

bool FWK::GraphicsDevice::CreateFactory()
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

bool FWK::GraphicsDevice::CreateDevice()
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

	return false;
}