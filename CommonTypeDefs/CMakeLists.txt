cmake_minimum_required (VERSION 3.8)

add_library (CommonTypeDefs
	"public/AppEntryTypeDef.h"
	"private/AppEntryTypeDef.cpp"
	"public/AppEntryObj.h"
	"private/AppEntryObj.cpp"
)

target_include_directories(CommonTypeDefs INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(CommonTypeDefs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(CommonTypeDefs PUBLIC Types)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CommonTypeDefs PROPERTY CXX_STANDARD 20)
endif()
