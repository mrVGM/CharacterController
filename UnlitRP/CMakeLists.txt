cmake_minimum_required (VERSION 3.8)

add_library (UnlitRP
	"public/UnlitRP.h"
	"private/UnlitRP.cpp"
	"public/UnlitMaterial.h"
	"private/UnlitMaterial.cpp"
)

target_include_directories(UnlitRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(UnlitRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(UnlitRP PUBLIC
	RenderPass
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET UnlitRP PROPERTY CXX_STANDARD 20)
endif()
