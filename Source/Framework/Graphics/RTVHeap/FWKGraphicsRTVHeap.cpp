#include "FWKGraphicsRTVHeap.h"

bool FWK::Graphics::RTVHeap::Create(ID3D12Device* a_device, const int a_useCount)
{
	D3D12_DESCRIPTOR_HEAP_DESC l_heapDesc = {};

	l_heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	 // レンダーターゲットとして扱う
	l_heapDesc.NodeMask       = 0;
	l_heapDesc.NumDescriptors = a_useCount;
	l_heapDesc.Flags		  = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // 特に指定なし

	auto l_hr = a_device->CreateDescriptorHeap(&l_heapDesc , IID_PPV_ARGS(&m_heap));

	if (FAILED(l_hr))
	{
		return false;
	}

	m_useCount      = a_useCount;
	m_incrementSize = a_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_device        = a_device;

	return true;
}

int FWK::Graphics::RTVHeap::CreateRTV(ID3D12Resource* a_buffer)
{
	if (m_useCount < m_nextRegisterNumber)
	{
		assert(false && "確保済みのヒープ領域を超えました。");
		return 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE l_handle = m_heap->GetCPUDescriptorHandleForHeapStart();
	l_handle.ptr += (UINT64)m_nextRegisterNumber * m_incrementSize;

	D3D12_RENDER_TARGET_VIEW_DESC l_rtvDesc = {};
	l_rtvDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	l_rtvDesc.ViewDimension					= D3D12_RTV_DIMENSION_TEXTURE2D;

	m_device->CreateRenderTargetView(a_buffer , &l_rtvDesc , l_handle);
	return m_nextRegisterNumber++;
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::RTVHeap::GetRTVCPUHandle(const int a_number)
{
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}
