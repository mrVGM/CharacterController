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
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Value m_camBuffer;
		Value m_dsDescriptorHeap;

		virtual void CreatePipelineStateAndRootSignatureForStaticMesh();

	protected:
		void LoadData(jobs::Job* done) override;

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
	};

}