#include "DXShader.h"

#include "PrimitiveTypes.h"
#include "AssetTypeDef.h"

#include "Files.h"
#include "Hash.h"

#include "Jobs.h"

#include "Settings.h"

#include <d3dcompiler.h>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}


namespace
{
	BasicObjectContainer<rendering::DXShaderTypeDef> m_shaderTypeDef;
	BasicObjectContainer<rendering::DXVertexShaderTypeDef> m_vertexShaderTypeDef;
	BasicObjectContainer<rendering::DXPixelShaderTypeDef> m_pixelShaderTypeDef;


	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
	{
		if (!srcFile || !entryPoint || !profile || !blob)
			return E_INVALIDARG;

		*blob = nullptr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, profile,
			flags, 0, &shaderBlob, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}

		*blob = shaderBlob;

		return hr;
	}
}

rendering::DXShaderTypeDef::DXShaderTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "7D55F6EB-BDB6-4199-83E8-5069BE1EB80F"),
	m_shaderName("130EF692-9CC6-4C8C-89B2-49A5D2F8EED2", StringTypeDef::GetTypeDef()),
	m_hash("8C20F797-3766-426A-9094-215F560FAB26", StringTypeDef::GetTypeDef())
{
	{
		m_shaderName.m_name = "Shader Name";
		m_shaderName.m_category = "Setup";
		m_shaderName.m_getValue = [](CompositeValue* obj) -> Value& {
			DXShader* shader = static_cast<DXShader*>(obj);
			return shader->m_name;
		};
		m_properties[m_shaderName.GetId()] = &m_shaderName;
	}

	{
		m_hash.m_name = "Hash";
		m_hash.m_category = "Internal";
		m_hash.m_getValue = [](CompositeValue* obj) -> Value& {
			DXShader* shader = static_cast<DXShader*>(obj);
			return shader->m_hash;
		};
		m_properties[m_hash.GetId()] = &m_hash;
	}

	m_name = "Shader";
	m_category = "Rendering";
}

rendering::DXShaderTypeDef::~DXShaderTypeDef()
{
}

const rendering::DXShaderTypeDef& rendering::DXShaderTypeDef::GetTypeDef()
{
	if (!m_shaderTypeDef.m_object)
	{
		m_shaderTypeDef.m_object = new DXShaderTypeDef();
	}

	return *m_shaderTypeDef.m_object;
}

rendering::DXVertexShaderTypeDef::DXVertexShaderTypeDef() :
	ReferenceTypeDef(&rendering::DXShaderTypeDef::GetTypeDef(), "F2A064DD-D5B3-44B4-94B5-AFC68840828D")
{
	m_name = "Vertex Shader";
	m_category = "Rendering";
}

void rendering::DXVertexShaderTypeDef::Construct(Value& container) const
{
	DXShader* shader = new DXShader(GetTypeDef());
	container.AssignObject(shader);
}

rendering::DXVertexShaderTypeDef::~DXVertexShaderTypeDef()
{
}

const rendering::DXVertexShaderTypeDef& rendering::DXVertexShaderTypeDef::GetTypeDef()
{
	if (!m_vertexShaderTypeDef.m_object)
	{
		m_vertexShaderTypeDef.m_object = new DXVertexShaderTypeDef();
	}

	return *m_vertexShaderTypeDef.m_object;
}

rendering::DXPixelShaderTypeDef::DXPixelShaderTypeDef() :
	ReferenceTypeDef(&rendering::DXShaderTypeDef::GetTypeDef(), "3823F212-4D44-453B-9DB9-F270A15FD135")
{
	m_name = "Pixel Shader";
	m_category = "Rendering";
}

rendering::DXPixelShaderTypeDef::~DXPixelShaderTypeDef()
{
}

void rendering::DXPixelShaderTypeDef::Construct(Value& container) const
{
	DXShader* shader = new DXShader(GetTypeDef());
	container.AssignObject(shader);
}

const rendering::DXPixelShaderTypeDef& rendering::DXPixelShaderTypeDef::GetTypeDef()
{
	if (!m_pixelShaderTypeDef.m_object)
	{
		m_pixelShaderTypeDef.m_object = new DXPixelShaderTypeDef();
	}

	return *m_pixelShaderTypeDef.m_object;
}

rendering::DXShader::DXShader(const ReferenceTypeDef& type) :
	ObjectValue(type),
	m_name(StringTypeDef::GetTypeDef(), this),
	m_hash(DXShaderTypeDef::GetTypeDef().m_hash.GetType(), this)
{
}

rendering::DXShader::~DXShader()
{
}

void rendering::DXShader::LoadData(jobs::Job done)
{
	jobs::Job compileJob = [=]() {
		std::string shaderFileName = m_name.Get<std::string>();
		std::string shaderFile = files::GetDataDir() + "Shaders\\src\\" + shaderFileName;

		std::string shaderCode;
		files::ReadTextFile("Shaders\\src\\" + m_name.Get<std::string>(), shaderCode);
		std::string hash = crypto::HashString(shaderFile + shaderCode);

		std::wstring shaderFileW(shaderFile.begin(), shaderFile.end());

		std::string shaderBinFileName = files::GetDataDir() + files::GetAssetsBinDir() + GetTypeDef().GetId() + ".bin";
		std::wstring shaderBinFileNameW(shaderBinFileName.begin(), shaderBinFileName.end());

		std::string cachedHash = m_hash.Get<std::string>();
		if (settings::UseBinFiles() && cachedHash == hash)
		{
			D3DReadFileToBlob(shaderBinFileNameW.c_str(), &m_shader);
			jobs::RunSync(done);
			return;
		}

		const char* entryPointVS = "VSMain";
		const char* entryPointPS = "PSMain";

		const char* profileVS = "vs_5_0";
		const char* profilePS = "ps_5_0";

		THROW_ERROR(CompileShader(
			shaderFileW.c_str(),
			TypeDef::IsA(GetTypeDef(), DXVertexShaderTypeDef::GetTypeDef()) ? entryPointVS : entryPointPS,
			TypeDef::IsA(GetTypeDef(), DXVertexShaderTypeDef::GetTypeDef()) ? profileVS : profilePS,
			&m_shader),
			"Can't compile shader!")

		
		D3DWriteBlobToFile(m_shader.Get(), shaderBinFileNameW.c_str(), true);

		{
			const AssetTypeDef& assetTypeDef = static_cast<const AssetTypeDef&>(GetTypeDef());
			json_parser::JSONValue& data = const_cast<json_parser::JSONValue&>(assetTypeDef.GetJSONData());
			auto& map = data.GetAsObj();
			auto& defaults = map["defaults"].GetAsObj();
			defaults[DXShaderTypeDef::GetTypeDef().m_hash.GetId()] = json_parser::JSONValue(hash);
			assetTypeDef.SaveJSONData();
		}

		jobs::RunSync(done);
	};

	jobs::RunAsync(compileJob);
}

ID3DBlob* rendering::DXShader::GetCompiledShader() const
{
	return m_shader.Get();
}

#undef THROW_ERROR