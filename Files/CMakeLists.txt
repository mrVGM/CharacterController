cmake_minimum_required (VERSION 3.8)

add_library (Files
	"public/Files.h"
	"private/Files.cpp"
	"public/MemoryFile.h"
	"private/MemoryFile.cpp"
)

target_include_directories(Files INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Files PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Files PROPERTY CXX_STANDARD 20)
endif()
