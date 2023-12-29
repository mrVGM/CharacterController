#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"
#include "MultiLoader.h"

#include "d3dx12.h"
#include <d3d12.h>
#include <wrl.h>

namespace rendering
{
	class DXShaderTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DXShaderTypeDef)

	public:
		TypeProperty m_shaderName;
		TypeProperty m_hash;

		DXShaderTypeDef();
		virtual ~DXShaderTypeDef();
	};

	class DXVertexShaderTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DXVertexShaderTypeDef)

	public:
		DXVertexShaderTypeDef();
		virtual ~DXVertexShaderTypeDef();

		virtual void Construct(Value& container) const override;
	};

	class DXPixelShaderTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DXPixelShaderTypeDef)

	public:
		DXPixelShaderTypeDef();
		virtual ~DXPixelShaderTypeDef();

		virtual void Construct(Value& container) const override;
	};

	class DXShader : public ObjectValue, public jobs::LoadingClass
	{
	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_shader;

		jobs::MultiLoader m_loader;


	protected:
		void LoadData(jobs::Job* done) override;

	public:
		Value m_name;
		Value m_hash;

		DXShader(const ReferenceTypeDef& type);
		virtual ~DXShader();

		void Load(jobs::Job* done);

		ID3DBlob* GetCompiledShader() const;
	};

}