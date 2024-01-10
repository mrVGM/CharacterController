#pragma once

#include "d3dx12.h"

namespace rendering::materials
{
	void Get3DMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& inputElementDescs, unsigned int& inputElementsCount);
	void Get3DSkeletalMeshMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& inputElementDescs, unsigned int& inputElementsCount);
}