﻿cmake_minimum_required (VERSION 3.8)

add_library (AssetManager
	"public/AssetManager.h"
	"private/AssetManager.cpp"
)

target_include_directories(AssetManager INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(AssetManager PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(AssetManager PUBLIC Files JSONParser Types Jobs)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AssetManager PROPERTY CXX_STANDARD 20)
endif()
