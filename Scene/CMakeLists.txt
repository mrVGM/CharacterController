cmake_minimum_required (VERSION 3.8)

add_library (Scene
	"public/Scene.h"
	"private/Scene.cpp"
)

target_include_directories(Scene INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Scene PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Scene PUBLIC
	RenderingCore
	Geometry
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Scene PROPERTY CXX_STANDARD 20)
endif()
