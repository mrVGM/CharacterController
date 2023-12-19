﻿cmake_minimum_required (VERSION 3.8)

add_library (RenderingCore
	"public/d3dx12.h"
	"public/WindowObj.h"
	"private/WindowObj.cpp"
	"public/WindowTypeDef.h"
	"private/WindowTypeDef.cpp"
	"public/RenderingCore.h"
	"private/RenderingCore.cpp"
	"public/RenderingEntryObj.h"
	"private/RenderingEntryObj.cpp"
	"public/RenderingEntryTypeDef.h"
	"private/RenderingEntryTypeDef.cpp"
	"public/DXShader.h"
	"private/DXShader.cpp"
)

target_include_directories(RenderingCore INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(RenderingCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(RenderingCore PUBLIC Jobs Types CommonTypeDefs)

target_link_libraries(RenderingCore PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib" "dxguid.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RenderingCore PROPERTY CXX_STANDARD 20)
endif()