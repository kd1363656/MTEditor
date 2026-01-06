#include "FWKRootSignature.h"

void FWK::Graphics::RootSignature::Create(FWK::Graphics::GraphicsDevice* a_graphicsDevice, const std::vector<FWK::CommonEnum::RangeType>& a_rangeTypeList)
{
	m_device = a_graphicsDevice;

	if (!m_device)
	{
		return;
	}

	D3D12_ROOT_SIGNATURE_DESC l_rootSignatureDesc = {};
	auto					  l_rangeCount        = (int)a_rangeTypeList.size();

	// レンジ数分だけルートパラメータ、レンジを生成
	std::vector<D3D12_ROOT_PARAMETER>   l_rootParams(l_rangeCount);
	std::vector<D3D12_DESCRIPTOR_RANGE> l_ranges    (l_rangeCount);

	// "SRV"の数だけ"SamplerDesc"を生成
	int l_samplerCount = 0;

	for (int l_i = 0; l_i < (int)a_rangeTypeList.size(); ++l_i)
	{
		if (a_rangeTypeList[l_i] != FWK::CommonEnum::RangeType::SRV) { continue; }
		++l_samplerCount;
	}

	// 指定された順に設定
	l_samplerCount   = 0;
	bool l_isSampler = false;
	int  l_cbvCount  = 0;
	int  l_uavCount  = 0;

	for (int l_i = 0; l_i < l_rangeCount; ++l_i)
	{
		switch(a_rangeTypeList[l_i])
		{
			case FWK::CommonEnum::RangeType::CBV:
				CreateRange(l_ranges[l_i] , FWK::CommonEnum::RangeType::CBV , l_cbvCount);
				l_rootParams[l_i].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				l_rootParams[l_i].DescriptorTable.pDescriptorRanges   = &l_ranges[l_i];
				l_rootParams[l_i].DescriptorTable.NumDescriptorRanges = 1;
				l_rootParams[l_i].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
				++l_cbvCount;
			break;

			case FWK::CommonEnum::RangeType::SRV:
				CreateRange(l_ranges[l_i] , FWK::CommonEnum::RangeType::SRV , l_samplerCount);
				l_rootParams[l_i].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				l_rootParams[l_i].DescriptorTable.pDescriptorRanges   = &l_ranges[l_i];
				l_rootParams[l_i].DescriptorTable.NumDescriptorRanges = 1;
				l_rootParams[l_i].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
				++l_samplerCount;

				l_isSampler = true;

			break;

			case FWK::CommonEnum::RangeType::UAV:
				CreateRange(l_ranges[l_i] , FWK::CommonEnum::RangeType::UAV , l_uavCount);
				l_rootParams[l_i].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				l_rootParams[l_i].DescriptorTable.pDescriptorRanges   = &l_ranges[l_i];
				l_rootParams[l_i].DescriptorTable.NumDescriptorRanges = 1;
				l_rootParams[l_i].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
				++l_uavCount;
			break;

			default:
			break;
		}
	}

	std::array<D3D12_STATIC_SAMPLER_DESC , 4> l_staticSamplerDescs = {};

	if (l_isSampler)
	{
		CreateStaticSampler(l_staticSamplerDescs[0] , TextureAddressMode::Wrap  , D3D12Filter::Point  , 0);
		CreateStaticSampler(l_staticSamplerDescs[1] , TextureAddressMode::Clamp , D3D12Filter::Point  , 1);
		CreateStaticSampler(l_staticSamplerDescs[2] , TextureAddressMode::Wrap  , D3D12Filter::Linear , 2);
		CreateStaticSampler(l_staticSamplerDescs[3] , TextureAddressMode::Clamp , D3D12Filter::Linear , 3);
	}

	l_rootSignatureDesc.pStaticSamplers   = l_isSampler ? l_staticSamplerDescs.data() : nullptr;
	l_rootSignatureDesc.NumStaticSamplers = l_isSampler ? 4 : 0;
	l_rootSignatureDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	l_rootSignatureDesc.pParameters       = l_rootParams.data();
	l_rootSignatureDesc.NumParameters     = (int)a_rangeTypeList.size();

	ID3DBlob* l_errorBlob = nullptr;
	auto      l_hr = D3D12SerializeRootSignature(&l_rootSignatureDesc           , 
												 D3D_ROOT_SIGNATURE_VERSION_1_0 ,
												 &m_rootBlob				    , 
												 &l_errorBlob);

	if (FAILED(l_hr))
	{
		assert(false && "ルートシグネチャ初期化失敗");
		return;
	}

	auto* l_device = m_device->GetDevice();
	if (!l_device) { return; }

	l_hr = l_device->CreateRootSignature(0							    ,
										 m_rootBlob->GetBufferPointer() , 
										 m_rootBlob->GetBufferSize()    , 
										 IID_PPV_ARGS(&m_rootSignature));

	if (FAILED(l_hr))
	{
		assert(false && "ルートシグネチャ作成失敗");
		return;
	}
}

void FWK::Graphics::RootSignature::CreateRange(D3D12_DESCRIPTOR_RANGE& a_range , const FWK::CommonEnum::RangeType a_type , const int a_count)
{
	switch(a_type)
	{
		case FWK::CommonEnum::RangeType::CBV:
			a_range				                      = {};
			a_range.NumDescriptors                    = 1;
			a_range.RangeType	                      = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			a_range.BaseShaderRegister                = a_count;
			a_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;

		case FWK::CommonEnum::RangeType::SRV:
			a_range				                      = {};
			a_range.NumDescriptors                    = 1;
			a_range.RangeType	                      = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			a_range.BaseShaderRegister                = a_count;
			a_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;

		case FWK::CommonEnum::RangeType::UAV:
			a_range				                      = {};
			a_range.NumDescriptors                    = 1;
			a_range.RangeType	                      = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			a_range.BaseShaderRegister                = a_count;
			a_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;

		default:
		break;
	}
}

void FWK::Graphics::RootSignature::CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC&						      a_samplerDesc ,
													   const FWK::Graphics::RootSignature::TextureAddressMode a_mode        ,
													   const FWK::Graphics::RootSignature::D3D12Filter		  a_filter      , 
													   const int											  a_count) const
{
	D3D12_TEXTURE_ADDRESS_MODE l_addressMode    = a_mode   == FWK::Graphics::RootSignature::TextureAddressMode::Wrap ? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	D3D12_FILTER			   l_samplingFilter = a_filter == FWK::Graphics::RootSignature::D3D12Filter::Point       ? D3D12_FILTER_MIN_MAG_MIP_POINT  : D3D12_FILTER_MIN_MAG_MIP_LINEAR;

	a_samplerDesc                  = {};
	a_samplerDesc.AddressU         = l_addressMode;
	a_samplerDesc.AddressV         = l_addressMode;
	a_samplerDesc.AddressW         = l_addressMode;
	a_samplerDesc.BorderColor      = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	a_samplerDesc.Filter           = l_samplingFilter;
	a_samplerDesc.MaxLOD           = D3D12_FLOAT32_MAX;
	a_samplerDesc.MinLOD           = 0.0f;
	a_samplerDesc.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	a_samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	a_samplerDesc.MaxAnisotropy    = 16U;
	a_samplerDesc.ShaderRegister   = a_count;
}