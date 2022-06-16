cmake_minimum_required(VERSION 3.1.0)

set(BL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Box2D
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_USER_SETTINGS OFF CACHE BOOL "" FORCE)
add_subdirectory("${BL_ROOT_DIR}/libs/box2d")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${BL_ROOT_DIR}/libs/glfw")
add_compile_definitions(GLFW_INCLUDE_NONE)


# GLAD
add_subdirectory("${BL_ROOT_DIR}/libs/glad")

# GLM
add_subdirectory("${BL_ROOT_DIR}/libs/glm")

# ImGui
set(IMGUI_SRC
	"${BL_ROOT_DIR}/libs/imgui/imconfig.h"
	"${BL_ROOT_DIR}/libs/imgui/imgui.h"
	"${BL_ROOT_DIR}/libs/imgui/imgui.cpp"
	"${BL_ROOT_DIR}/libs/imgui/imgui_internal.h"
	"${BL_ROOT_DIR}/libs/imgui/imgui_demo.cpp"
	"${BL_ROOT_DIR}/libs/imgui/imgui_draw.cpp"
	"${BL_ROOT_DIR}/libs/imgui/imgui_tables.cpp"
	"${BL_ROOT_DIR}/libs/imgui/imgui_widgets.cpp"
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC "${BL_ROOT_DIR}/libs/imgui")

# Blobolution
project(Blobolution)
set(BL_SRC
	"${BL_ROOT_DIR}/src/BlobolutionPch.pch"
	"${BL_ROOT_DIR}/src/Main.cpp"
	"${BL_ROOT_DIR}/src/Log.h"
	"${BL_ROOT_DIR}/src/Event.h"
	"${BL_ROOT_DIR}/src/Layer.h"
	"${BL_ROOT_DIR}/src/Random.h"
	"${BL_ROOT_DIR}/src/Random.cpp"
	"${BL_ROOT_DIR}/src/Application.h"
	"${BL_ROOT_DIR}/src/Application.cpp"
	"${BL_ROOT_DIR}/src/Window.h"
	"${BL_ROOT_DIR}/src/Window.cpp"
	"${BL_ROOT_DIR}/src/Renderer.h"
	"${BL_ROOT_DIR}/src/Renderer.cpp"
	"${BL_ROOT_DIR}/src/SimLayer.h"
	"${BL_ROOT_DIR}/src/SimLayer.cpp"
	"${BL_ROOT_DIR}/src/Generation.h"
	"${BL_ROOT_DIR}/src/Generation.cpp"
	"${BL_ROOT_DIR}/src/Platform.h"
	"${BL_ROOT_DIR}/src/Platform.cpp"
	"${BL_ROOT_DIR}/src/Car.h"
	"${BL_ROOT_DIR}/src/Car.cpp"
	"${BL_ROOT_DIR}/src/ImGuiBuild.cpp"
)
add_executable(Blobolution ${BL_SRC})
target_precompile_headers(Blobolution PRIVATE "${BL_ROOT_DIR}/src/BlobolutionPch.pch")
target_include_directories(Blobolution PRIVATE "${BL_ROOT_DIR}/src")
target_link_libraries(Blobolution PRIVATE glfw glad glm box2d imgui)


# Resources
set(BL_RES
)
file(COPY ${BL_RES} DESTINATION "res")