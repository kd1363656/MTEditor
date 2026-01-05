#include "FWKMesh.h"

void FWK::Graphics::Mesh::Create(FWK::Graphics::GraphicsDevice* a_graphicsDevice)
{
	m_device = a_graphicsDevice;

	// 頂点の座標
	m_vertices[0] = { -1.0F , 1.0F };
	m_vertices[1] = {  1.0F , 1.0F };
	m_vertices[2] = {  0.0F , 1.0F };
	
	D3D12_HEAP_PROPERTIES l_heapProp = {};
	l_heapProp.Type					 = D3D12_HEAP_TYPE_UPLOAD;
	l_heapProp.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	l_heapProp.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC l_resDesc = {};
	l_resDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
	l_resDesc.Width               = sizeof(FWK::Math::Vector3) * m_vertices.size();
	l_resDesc.Height              = 1;
	l_resDesc.DepthOrArraySize    = 1;
	l_resDesc.MipLevels           = 1;
	l_resDesc.Format              = DXGI_FORMAT_UNKNOWN;
	l_resDesc.SampleDesc.Count    = 1;
	l_resDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
	l_resDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto* l_hr = m_device->GetDevice();
	if (!l_hr) { return; }

	l_hr->CreateCommittedResource(&l_heapProp                       , 
								  D3D12_HEAP_FLAG_NONE              , 
								  &l_resDesc		                , 
								  D3D12_RESOURCE_STATE_GENERIC_READ , 
								  nullptr                           ,
								  IID_PPV_ARGS(&m_vBuffer));

	if (FAILED(l_hr))
	{
		assert(false && "頂点バッファー作成失敗");
		return;
	}

	m_vBView.BufferLocation = m_vBuffer->GetGPUVirtualAddress();
	m_vBView.SizeInBytes    = (UINT)l_resDesc.Width;
	m_vBView.StrideInBytes  = sizeof(FWK::Math::Vector3);

	// 頂点バッファーに情報を書き込む
	FWK::Math::Vector3* l_vBMap = nullptr;
	{
		m_vBuffer->Map  (0					    , nullptr			   , (void**)&l_vBMap);
		std::copy       (std::begin(m_vertices) , std::end(m_vertices) , l_vBMap);
		m_vBuffer->Unmap(0						, nullptr);
	}
}

void FWK::Graphics::Mesh::DrawInstanced() const
{
	if (!m_device) { return; }

	m_device->GetCmdList()->IASetVertexBuffers(0 , 1 , &m_vBView);
	m_device->GetCmdList()->DrawInstanced     (3 , 1 , 0 , 0);
}