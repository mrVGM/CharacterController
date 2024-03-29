﻿cmake_minimum_required (VERSION 3.8)

add_library (RenderingCore
	"public/d3dx12.h"
	"public/RenderWindow.h"
	"private/RenderWindow.cpp"
	"public/RenderingCore.h"
	"private/RenderingCore.cpp"
	"public/DXShader.h"
	"private/DXShader.cpp"
	"public/DXDevice.h"
	"private/DXDevice.cpp"
	"public/DXSwapChain.h"
	"private/DXSwapChain.cpp"
	
	
	"private/DXCommandQueue.cpp"
	"public/DXCommandQueue.h"
	"public/DXFence.h"
	"private/DXFence.cpp"
	"private/WaitFence.cpp"
	"public/WaitFence.h"
	"public/DXHeap.h"
	"private/DXHeap.cpp"
	"private/ResidentHeapFence.h"
	"private/ResidentHeapFence.cpp"
	"private/ResidentHeapJobSystem.h"
	"private/ResidentHeapJobSystem.cpp"
	"public/RenderFence.h"
	"private/RenderFence.cpp"
	"public/DXBuffer.h"
	"private/DXBuffer.cpp"
	"public/DXCopyBuffers.h"
	"private/DXCopyBuffers.cpp"
	"public/DXMutableBuffer.h"
	"private/DXMutableBuffer.cpp"
	"public/DXTexture.h"
	"private/DXTexture.cpp"
	"public/DXDepthStencilTexture.h"
	"private/DXDepthStencilTexture.cpp"
	"public/DXDescriptorHeap.h"
	"private/DXDescriptorHeap.cpp"
 )

target_include_directories(RenderingCore INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(RenderingCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(RenderingCore PUBLIC Jobs Types CommonTypeDefs Settings)

target_link_libraries(RenderingCore PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib" "dxguid.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RenderingCore PROPERTY CXX_STANDARD 20)
endif()
