﻿cmake_minimum_required (VERSION 3.8)

add_library (Game
	"public/PlayerController.h"
	"private/PlayerController.cpp"
	"public/Game.h"
	"private/Game.cpp"
	"public/Character.h"
	"private/Character.cpp"
	"private/CharacterAnimator.cpp"
	"public/CharacterAnimator.h"
)

target_include_directories(Game INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Game PUBLIC
	Runtime
	Renderer
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Game PROPERTY CXX_STANDARD 20)
endif()
