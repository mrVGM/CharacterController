cmake_minimum_required (VERSION 3.8)

add_library (Geometry
	"public/Mesh.h"
	"private/Mesh.cpp"
	"public/Geometry.h"
	"private/Geometry.cpp"
	
	"public/GeometryAppEntry.h"
 "private/GeometryAppEntry.cpp")

target_include_directories(Geometry INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Geometry PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Geometry PUBLIC
	Types
	Files
	XMLReader
	JSONParser
	Jobs
	CommonTypeDefs
	Math
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Geometry PROPERTY CXX_STANDARD 20)
endif()
