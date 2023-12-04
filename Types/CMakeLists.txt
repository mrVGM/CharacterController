cmake_minimum_required (VERSION 3.8)

add_library (Types
	"public/Types.h"
	"private/Types.cpp"
	"public/TypeDef.h"
	"private/TypeDef.cpp"
	"public/PrimitiveTypes.h"
	"private/PrimitiveTypes.cpp"
	"public/CompositeTypeDef.h"
	"private/CompositeTypeDef.cpp"
	"public/Object.h"
	"private/Object.cpp"
	"public/GenericListTypeDef.h"
	"private/GenericListTypeDef.cpp"
	"public/ListTypeDef.h"
	"private/ListTypeDef.cpp"
	"public/GenericList.h"
)

target_include_directories(Types INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Types PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Types PUBLIC JSONParser)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Types PROPERTY CXX_STANDARD 20)
endif()
