#pragma once

namespace FWK::Graphics
{
	class Mesh
	{
	public:

		Mesh () = default;
		~Mesh() = default;

		void Create(FWK::Graphics::GraphicsDevice* a_graphicsDevice);

		// インスタンス描画
		void DrawInstanced() const;

	private:

		FWK::Graphics::GraphicsDevice* m_device  = nullptr;
		ComPtr<ID3D12Resource>         m_vBuffer = nullptr;
		ComPtr<ID3D12Resource>         m_IBuffer = nullptr;

		D3D12_VERTEX_BUFFER_VIEW m_vBView;
		D3D12_INDEX_BUFFER_VIEW  m_iBVew;

		std::array<Math::Vector3, 3> m_vertices;
	};
}