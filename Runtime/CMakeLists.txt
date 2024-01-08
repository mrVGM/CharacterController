cmake_minimum_required (VERSION 3.8)

add_library (Runtime
	"public/Runtime.h"
	"private/Runtime.cpp"
	"private/TickUpdater.cpp"
	"public/TickUpdater.h"
	"private/Actor.cpp"
	"public/Actor.h"
	"private/MeshBuffers.cpp"
	"public/MeshBuffers.h"
	"private/Animator.cpp"
	"public/Animator.h"
	"public/PlayerController.h"
	"private/PlayerController.cpp"
	"public/Input.h"
	"private/Input.cpp"
)

target_include_directories(Runtime INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Runtime PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Runtime PUBLIC
	Types
	Jobs
	Geometry
	RenderingCore
	Materials
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Runtime PROPERTY CXX_STANDARD 20)
endif()
