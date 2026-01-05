#pragma once

namespace FWK::Graphics
{
	class RTVHeap final
	{
	public:

		RTVHeap () = default;
		~RTVHeap() = default;

		// ヒープ作成
		bool Create(ID3D12Device* a_device , const int a_useCount);

		// "RTV"の作成
		int CreateRTV(ID3D12Resource* a_buffer);

		// "RTV"の"CPU"側アドレス
		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle(const int a_number) const;

	private:
		
		ComPtr<ID3D12DescriptorHeap> m_heap   = nullptr;
		ID3D12Device*				 m_device = nullptr;

		int m_useCount           = 0;
		int m_incrementSize      = 0;
		int m_nextRegisterNumber = 0;
	};
}