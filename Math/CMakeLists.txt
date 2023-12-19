cmake_minimum_required (VERSION 3.8)

add_library (Math
	"public/MathUtils.h"
	"private/MathUtils.cpp"
)

target_include_directories(Math INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Math PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Math PROPERTY CXX_STANDARD 20)
endif()
