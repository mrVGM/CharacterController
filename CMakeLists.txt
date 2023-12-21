# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VisualScriptingEngine")

add_definitions(-DUNICODE)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
ENDIF()

# Include sub-projects.
add_subdirectory ("Main")
add_subdirectory ("Files")
add_subdirectory ("Parser")
add_subdirectory ("JSONParser")
add_subdirectory ("AssetManager")
add_subdirectory ("Types")
add_subdirectory ("GC")
add_subdirectory ("GCWorker")
add_subdirectory ("App")
add_subdirectory ("Reflection")
add_subdirectory ("Jobs")
add_subdirectory ("CommonTypeDefs")
add_subdirectory ("RenderingCore")
add_subdirectory ("XMLReader")
add_subdirectory ("Math")
add_subdirectory ("Geometry")
