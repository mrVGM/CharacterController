cmake_minimum_required (VERSION 3.8)

add_library (GCWorker
	"public/GCWorker.h"
	"private/GCWorker.cpp"
)

target_include_directories(GCWorker INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(GCWorker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(GCWorker PUBLIC
	Types
	Jobs
	GC
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GCWorker PROPERTY CXX_STANDARD 20)
endif()
