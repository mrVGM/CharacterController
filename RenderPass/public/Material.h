#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include "d3dx12.h"

#include "DXBuffer.h"

#include <wrl.h>

namespace rendering::render_pass
{
	class MaterialTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(MaterialTypeDef)

	public:
		TypeProperty m_vertexShader;
		TypeProperty m_pixelShader;

		MaterialTypeDef();
		virtual ~MaterialTypeDef();

		void Construct(Value& container) const override;
	};

	class Material : public ObjectValue
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Value m_device;
		Value m_swapChain;
		Value m_vertexShader;
		Value m_pixelShader;

		virtual void CreatePipelineStateAndRootSignatureForStaticMesh();

	public:
		Value m_vertexShaderDef;
		Value m_pixelShaderDef;

		Material(const ReferenceTypeDef& typeDef);
		virtual ~Material();

		virtual void GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			ID3D12GraphicsCommandList* commandList);

		virtual void Load(jobs::Job* done);
	};

}