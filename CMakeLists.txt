cmake_minimum_required(VERSION 3.1.0)

file(GLOB_RECURSE RES_FILES "res/*.vs" "res/*.fs" "res/*.jpg" "res/*.obj")
file(COPY ${RES_FILES} DESTINATION "res")

project(Blobolution)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

# Box2D
add_subdirectory(libs/box2d)

# Game
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
include_directories(src/)
add_executable(Game ${SRC_FILES})
target_link_libraries(Game PRIVATE glfw glad glm box2d)



