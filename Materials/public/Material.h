#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MultiLoader.h"
#include "Job.h"

#include "d3dx12.h"

#include "DXBuffer.h"

#include <wrl.h>

namespace rendering::materials
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

	class Material : public ObjectValue, public jobs::LoadingClass
	{
	private:
		jobs::MultiLoader m_loader;

	protected:
		Value m_device;
		Value m_swapChain;
		Value m_vertexShader;
		Value m_pixelShader;

		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_vertexShaderDef;
		Value m_pixelShaderDef;

		Material(const ReferenceTypeDef& typeDef);
		virtual ~Material();

		virtual void GenerateCommandList(
			const DXBuffer & vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			ID3D12CommandAllocator* commandAllocator,
			ID3D12GraphicsCommandList* commandList);

		virtual void Load(jobs::Job* done);
	};

}