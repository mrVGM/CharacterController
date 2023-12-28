cmake_minimum_required (VERSION 3.8)

add_library (Materials
	"public/Material.h"
	"private/Material.cpp"
 "public/Materials.h" "private/Materials.cpp")

target_include_directories(Materials INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Materials PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Materials PUBLIC
	Types
	Jobs
	RenderingCore
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Materials PROPERTY CXX_STANDARD 20)
endif()
