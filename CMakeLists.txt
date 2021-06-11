cmake_minimum_required(VERSION 3.1.0)

file(GLOB_RECURSE SRC_FILES 
	"src/*.cpp"
)
file(GLOB_RECURSE RES_FILES "res/*.vs" "res/*.fs" "res/*.jpg" "res/*.obj")
file(COPY ${RES_FILES} DESTINATION "res")

project(Blobolution)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Box2D
add_subdirectory(libs/box2d)
option(BOX2D_BUILD_UNIT_TESTS "Build the Box2D unit tests" OFF)
option(BOX2D_BUILD_TESTBED "Build the Box2D testbed" OFF)
option(BOX2D_BUILD_DOCS "Build the Box2D documentation" OFF)
option(BOX2D_USER_SETTINGS "Override Box2D settings with b2UserSettings.h" OFF)

# GLFW
add_subdirectory(libs/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_compile_definitions(GLFW_INCLUDE_NONE)

# GLAD
add_subdirectory(libs/glad)

# GLM
add_subdirectory(libs/glm)

# ImGui
file(GLOB IMGUI_SOURCES
		"libs/imgui/*.cpp"
		"libs/imgui/*.h"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "libs/imgui")

# Blobolution
include_directories(src/)
add_executable(Blobolution ${SRC_FILES})
target_link_libraries(Blobolution PRIVATE glfw glad glm box2d imgui)



