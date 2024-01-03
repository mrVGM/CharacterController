cmake_minimum_required (VERSION 3.8)

add_library (Settings
	"public/Settings.h"
	"private/Settings.cpp"
)

target_include_directories(Settings INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Settings PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Settings PROPERTY CXX_STANDARD 20)
endif()
