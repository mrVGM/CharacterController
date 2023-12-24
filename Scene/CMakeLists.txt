cmake_minimum_required (VERSION 3.8)

add_library (Scene
	"public/Scene.h"
	"private/Scene.cpp"
	"public/SceneObject.h"
	"private/SceneObject.cpp"
	"public/Actor.h"
	"private/Actor.cpp"
	"public/MeshBuffers.h"
	"private/MeshBuffers.cpp"
)

target_include_directories(Scene INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Scene PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Scene PUBLIC
	RenderingCore
	Geometry
	Jobs
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Scene PROPERTY CXX_STANDARD 20)
endif()
