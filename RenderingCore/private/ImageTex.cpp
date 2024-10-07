#include "ImageTex.h"

#include "DXDevice.h"
#include "DXTexture.h"
#include "DXHeap.h"
#include "DXBuffer.h"
#include "Files.h"
#include "DXCopyBuffers.h"
#include "ObjectValueContainer.h"
#include "PrimitiveTypes.h"

#include "d3dx12.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<rendering::ImageTexTypeDef> m_imageLoader;

	// get the dxgi format equivilent of a wic format
	DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		else return DXGI_FORMAT_UNKNOWN;
	}

	// get the number of bits per pixel for a dxgi format
	int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
	{
		if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

		else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
		else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
	}


	struct LoadImageCommandList
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		ID3D12CommandList* m_commandLists[1];

		LoadImageCommandList(Value& device)
		{
			using Microsoft::WRL::ComPtr;
			using namespace rendering;
			DXDevice* m_device = device.GetValue<DXDevice*>();

			THROW_ERROR(
				m_device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_commandAllocator)),
				"Can't create Command Allocator!")

			THROW_ERROR(
				m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
				"Can't create Command List!")

			THROW_ERROR(
				m_commandList->Close(),
				"Can't close command List!")

			m_commandLists[0] = m_commandList.Get();
		}
	};
}

const rendering::ImageTexTypeDef& rendering::ImageTexTypeDef::GetTypeDef()
{
	if (!m_imageLoader.m_object)
	{
		m_imageLoader.m_object = new ImageTexTypeDef();
	}

	return *m_imageLoader.m_object;
}

rendering::ImageTexTypeDef::ImageTexTypeDef() :
	ReferenceTypeDef(&DXTextureTypeDef::GetTypeDef(), "F8AF9A7B-8475-4924-AF85-8CFBD4710254"),
	m_file("27FDC497-19C8-4FAB-B2A8-F3113B941777", StringTypeDef::GetTypeDef())
{
	{
		m_file.m_name = "File";
		m_file.m_category = "Setup";
		m_file.m_getValue = [](CompositeValue* value) -> Value& {
			ImageTex* imageTex = static_cast<ImageTex*>(value);
			return imageTex->m_file;
		};
		m_properties[m_file.GetId()] = &m_file;
	}
	m_category = "Images";
	m_name = "Image Texture";
}

rendering::ImageTexTypeDef::~ImageTexTypeDef()
{
}

void rendering::ImageTexTypeDef::Construct(Value& container) const
{
	ImageTex* loader = new ImageTex();
	container.AssignObject(loader);
}

rendering::ImageTex::ImageTex() :
	DXTexture(rendering::ImageTexTypeDef::GetTypeDef()),
	m_file(StringTypeDef::GetTypeDef(), this)
{
	THROW_ERROR(
		CoInitialize(nullptr),
		"CoInitialize Error!"
	)

	THROW_ERROR(
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<LPVOID*>(m_factory.GetAddressOf())
		),
		"Can't Create WIC Imaging Factory!"
	)
}

rendering::ImageTex::~ImageTex()
{
}

void rendering::ImageTex::LoadImageFromFile(const std::string& imageFile, jobs::Job done)
{
	std::string fullPath = files::GetDataDir() + "Images\\src\\" + imageFile;
	std::wstring imageFileW(fullPath.begin(), fullPath.end());

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;

	HRESULT hr = m_factory->CreateDecoderFromFilename(
		imageFileW.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	hr = decoder->GetFrame(0, &frame);

	WICPixelFormatGUID pf;
	hr = frame->GetPixelFormat(&pf);

	DXGI_FORMAT format = GetDXGIFormatFromWICFormat(pf);

	if (format == DXGI_FORMAT_UNKNOWN)
	{
		throw "Unsupported image format!";
	}

	UINT width;
	UINT height;
	frame->GetSize(&width, &height);
	
	int bitsPerPixel = GetDXGIFormatBitsPerPixel(format); // number of bits per pixel
	int bytesPerRow = (width * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * height; // total image size in bytes

	struct CTX {
		int toLoad = 2;
		Value tex;
		Value buff;
		Value heap;
		BYTE* imageData = nullptr;
		CTX() :
			tex(DXTextureTypeDef::GetTypeDef(), nullptr),
			buff(DXBufferTypeDef::GetTypeDef(), nullptr),
			heap(DXHeapTypeDef::GetTypeDef(), nullptr)
		{
		}
		~CTX()
		{
			delete[] imageData;
		}
	};

	CTX* ctx = new CTX();
	ctx->imageData = new BYTE[imageSize];
	frame->CopyPixels(nullptr, bytesPerRow, imageSize, ctx->imageData);
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = ctx->imageData; // pointer to our image data
	textureData.RowPitch = bytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = bytesPerRow * height; // also the size of our triangle vertex data

	jobs::Job itemLoaded = [=]() {
		--ctx->toLoad;
		if (ctx->toLoad > 0)
		{
			return;
		}

		DXBuffer* buff = ctx->buff.GetValue<DXBuffer*>();
		DXHeap* heap = ctx->heap.GetValue<DXHeap*>();
		buff->Place(heap, 0);
		DXTexture* tex = ctx->tex.GetValue<DXTexture*>();

		Value dev(DXDeviceTypeDef::GetTypeDef(), nullptr);
		ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), dev);
		Value copyBuffers(DXCopyBuffersTypeDef::GetTypeDef(), nullptr);
		ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef(), copyBuffers);
		
		LoadImageCommandList* loadImageCommandList = new LoadImageCommandList(dev);
		loadImageCommandList->m_commandList.Get()->Reset(loadImageCommandList->m_commandAllocator.Get(), nullptr);
		UpdateSubresources(loadImageCommandList->m_commandList.Get(), tex->GetTexture(), buff->GetBuffer(), 0, 0, 1, &textureData);
		loadImageCommandList->m_commandList->Close();

		DXCopyBuffers* m_copyBuffers = copyBuffers.GetValue<DXCopyBuffers*>();
		m_copyBuffers->Execute(loadImageCommandList->m_commandLists, 1, [=]() {
			delete loadImageCommandList;
			delete ctx;
			jobs::RunSync(done);
		});
	};

	jobs::Job createTexAndBuffer = [=]() {
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		CD3DX12_RESOURCE_DESC textureDesc = {};
		textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			format,
			width,
			height,
			1,
			0,
			1,
			0,
			flags);

		DXTextureTypeDef::GetTypeDef().Construct(ctx->tex);

		DXTexture* t = ctx->tex.GetValue<DXTexture*>();
		t->SetDescription(textureDesc);
		t->Load(itemLoaded);

		Value dev(DXDeviceTypeDef::GetTypeDef(), nullptr);
		ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), dev);
		DXDevice* device = dev.GetValue<DXDevice*>();
		UINT64 textureBufferSize;
		device->GetDevice().GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureBufferSize);

		DXBufferTypeDef::GetTypeDef().Construct(ctx->buff);
		DXBuffer* buff = ctx->buff.GetValue<DXBuffer*>();
		buff->SetBufferSizeAndFlags(textureBufferSize, D3D12_RESOURCE_FLAG_NONE);
		buff->SetBufferStride(textureBufferSize);

		DXHeapTypeDef::GetTypeDef().Construct(ctx->heap);

		DXHeap* heap = ctx->heap.GetValue<DXHeap*>();
		heap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
		heap->SetHeapSize(textureBufferSize);
		heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
		heap->MakeResident(itemLoaded);
	};

	jobs::RunSync(createTexAndBuffer);
}

void rendering::ImageTex::LoadData(jobs::Job done)
{
	std::string file = m_file.Get<std::string>();

	LoadImageFromFile(file, done);
}

#undef THROW_ERROR