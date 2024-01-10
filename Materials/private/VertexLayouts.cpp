#include "VertexLayouts.h"

namespace
{
	D3D12_INPUT_ELEMENT_DESC m_3DMaterialInputlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_ELEMENT_DESC m_3DSkeletalMeshMaterialInputlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS_INDEX", 0, DXGI_FORMAT_R32_SINT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void rendering::materials::Get3DMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& inputElementDescs, unsigned int& inputElementsCount)
{
	inputElementDescs = m_3DMaterialInputlayout;
	inputElementsCount = _countof(m_3DMaterialInputlayout);
}

void rendering::materials::Get3DSkeletalMeshMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& inputElementDescs, unsigned int& inputElementsCount)
{
	inputElementDescs = m_3DSkeletalMeshMaterialInputlayout;
	inputElementsCount = _countof(m_3DSkeletalMeshMaterialInputlayout);
}
