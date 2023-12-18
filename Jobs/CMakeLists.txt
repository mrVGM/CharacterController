cmake_minimum_required (VERSION 3.8)

add_library (Jobs
	"public/JobSystemDef.h"
	"private/JobSystemDef.cpp"
	"public/JobSystem.h"
	"private/JobSystem.cpp"
	"public/Job.h"
	"private/Job.cpp"
	"private/Thread.h"
	"private/Thread.cpp"
	"public/Jobs.h"
	"private/Jobs.cpp"
)

target_include_directories(Jobs INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Jobs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Jobs PUBLIC Types AssetManager)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Jobs PROPERTY CXX_STANDARD 20)
endif()
