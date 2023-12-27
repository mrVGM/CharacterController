#pragma once

#include "Material.h"

namespace rendering::unlit_rp
{
	class UnlitMaterialTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(UnlitMaterialTypeDef)

	public:
		UnlitMaterialTypeDef();
		virtual ~UnlitMaterialTypeDef();

		void Construct(Value& container) const override;
	};

	class UnlitMaterial : public render_pass::Material
	{
	private:
		Value m_camBuffer;
		Value m_dsDescriptorHeap;

	public:
		UnlitMaterial(const ReferenceTypeDef& typeDef);
		virtual ~UnlitMaterial();

		void GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			ID3D12CommandAllocator* commandAllocator,
			ID3D12GraphicsCommandList* commandList) override;

		void Load(jobs::Job* done) override;
	};

}