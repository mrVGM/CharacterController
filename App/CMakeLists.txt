cmake_minimum_required (VERSION 3.8)

add_library (App
	"public/App.h"
	"private/App.cpp"
	"public/AppTypeDef.h"
	"private/AppTypeDef.cpp"
	"public/AppObj.h"
	"private/AppObj.cpp"
)

target_include_directories(App INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(App PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(App PUBLIC
	CommonTypeDefs
	AssetManager
	Reflection
	Jobs
	RenderingCore
	XMLReader
	Geometry
	GCWorker
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET App PROPERTY CXX_STANDARD 20)
endif()
