#pragma once

#include "Material.h"

namespace rendering::render_pass
{
	class DisplayTextureMaterialTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DisplayTextureMaterialTypeDef)

	public:
		TypeProperty m_texDescriptorHeap;

		DisplayTextureMaterialTypeDef();
		virtual ~DisplayTextureMaterialTypeDef();

		void Construct(Value& container) const override;
	};

	class DisplayTextureMaterial : public materials::Material
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Value m_texDescriptorHeap;

		virtual void CreatePipelineStateAndRootSignatureForStaticMesh();

	protected:
		void LoadData(jobs::Job done) override;

	public:
		Value m_texDescriptorHeapDef;

		DisplayTextureMaterial(const ReferenceTypeDef& typeDef);
		virtual ~DisplayTextureMaterial();

		void GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			ID3D12CommandAllocator* commandAllocator,
			ID3D12GraphicsCommandList* commandList) override;
	};
}