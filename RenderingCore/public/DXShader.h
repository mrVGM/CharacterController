#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

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

	class DXShader : public ObjectValue
	{
	private:
		bool m_loaded = false;
		Microsoft::WRL::ComPtr<ID3DBlob> m_shader;

	public:
		Value m_name;

		DXShader(const ReferenceTypeDef& type);
		virtual ~DXShader();

		void Load(jobs::Job* done);
		bool IsLoaded();

		ID3DBlob* GetCompiledShader() const;
	};

}