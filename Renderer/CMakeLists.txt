cmake_minimum_required (VERSION 3.8)

add_library (Renderer
	"public/Renderer.h"
	"private/Renderer.cpp"
	"private/RendererAppEntry.h"
	"private/RendererAppEntry.cpp"
	"public/TickUpdater.h"
	"private/TickUpdater.cpp"
	"public/Camera.h"
	"private/Camera.cpp"
)

target_include_directories(Renderer INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Renderer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Renderer PUBLIC
	RenderingCore
	App
	Types
	Scene
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Renderer PROPERTY CXX_STANDARD 20)
endif()
