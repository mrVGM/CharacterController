﻿cmake_minimum_required (VERSION 3.8)

add_library (GC
	"public/GC.h"
	"private/GC.cpp"
	"private/ObjectRecords.h"
	"private/ObjectRecords.cpp")

target_include_directories(GC INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(GC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GC PROPERTY CXX_STANDARD 20)
endif()
