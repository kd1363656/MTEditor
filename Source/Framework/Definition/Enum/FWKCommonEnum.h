#pragma once

namespace FWK::CommonEnum
{
	enum class RangeType
	{
		CBV ,
		SRV ,
		UAV ,
	};

	enum class CullMode
	{
		None  = D3D12_CULL_MODE_NONE  ,
		Front = D3D12_CULL_MODE_FRONT ,
		Back  = D3D12_CULL_MODE_BACK
	};

	enum class BlendMode
	{
		Add   , 
		Alpha ,
	};

	enum class InputLayout
	{
		POSITION   ,
		TEXCOORD   ,
		NORMAL     ,
		TANGENT    ,
		COLOR      ,
		SKININDEX  ,
		SKINWEIGHT ,
	};

	enum class PrimitiveTopologyType
	{
		Undefined = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED ,
		Point     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT     ,
		Line      = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE      ,
		Triangle  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE  ,
		Patch     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
	};
}