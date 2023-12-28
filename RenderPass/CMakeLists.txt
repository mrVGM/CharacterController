cmake_minimum_required (VERSION 3.8)

add_library (RenderPass
	"public/RenderPass.h"
	"private/RenderPass.cpp"
	"private/ClearScreenRP.h"
	"private/ClearScreenRP.cpp"
	"public/CameraBuffer.h"
	"private/CameraBuffer.cpp"
	"private/DisplayTextureMaterial.cpp"
	"public/DisplayTextureMaterial.h"
)

target_include_directories(RenderPass INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(RenderPass PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(RenderPass PUBLIC
	Types
	Jobs
	RenderingCore
	Materials
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RenderPass PROPERTY CXX_STANDARD 20)
endif()
