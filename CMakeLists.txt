cmake_minimum_required(VERSION 3.1.0)

set(BL_ROOT_DIR			${CMAKE_CURRENT_SOURCE_DIR})
set(BL_SRC_DIR			${BL_ROOT_DIR}/src)
set(BL_LIB_DIR			${BL_ROOT_DIR}/libs)
set(BL_RES_DIR			${BL_ROOT_DIR}/res)

project(Blobolution)


#--------------------------------------------------------------------------------------------------
#	Configuration
#--------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
add_compile_options(
	$<$<CONFIG:RELEASE>:/Ox>
	$<$<CONFIG:RELEASE>:/GL>
)
add_link_options(
	$<$<CONFIG:RELEASE>:/LTCG>
)
else()
add_compile_options(
	$<$<CONFIG:RELEASE>:-Ofast>
)
add_link_options(
	$<$<CONFIG:RELEASE>:-flto>
)
endif()


#--------------------------------------------------------------------------------------------------
#	Sources
#--------------------------------------------------------------------------------------------------
set(BL_HSP
	"${BL_SRC_DIR}/"
)


set(BL_SRC
	"${BL_SRC_DIR}/Prefix.pch"

	"${BL_SRC_DIR}/Main.cpp"
	"${BL_SRC_DIR}/Log.h"
	"${BL_SRC_DIR}/Event.h"
	"${BL_SRC_DIR}/Layer.h"
	"${BL_SRC_DIR}/Random.h"
	"${BL_SRC_DIR}/Random.cpp"
	"${BL_SRC_DIR}/Application.h"
	"${BL_SRC_DIR}/Application.cpp"
	"${BL_SRC_DIR}/Window.h"
	"${BL_SRC_DIR}/Window.cpp"
	"${BL_SRC_DIR}/Renderer.h"
	"${BL_SRC_DIR}/Renderer.cpp"
	"${BL_SRC_DIR}/SimLayer.h"
	"${BL_SRC_DIR}/SimLayer.cpp"
	"${BL_SRC_DIR}/Generation.h"
	"${BL_SRC_DIR}/Generation.cpp"
	"${BL_SRC_DIR}/Platform.h"
	"${BL_SRC_DIR}/Platform.cpp"
	"${BL_SRC_DIR}/Car.h"
	"${BL_SRC_DIR}/Car.cpp"
	"${BL_SRC_DIR}/ImGuiBuild.cpp"
)


#--------------------------------------------------------------------------------------------------
#	Libraries
#--------------------------------------------------------------------------------------------------
add_subdirectory("${BL_LIB_DIR}/glad")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${BL_LIB_DIR}/glfw")

add_subdirectory("${BL_LIB_DIR}/glm")

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_USER_SETTINGS OFF CACHE BOOL "" FORCE)
add_subdirectory("${BL_LIB_DIR}/box2d")

set(IMGUI_SRC
	"${BL_LIB_DIR}/imgui/imconfig.h"
	"${BL_LIB_DIR}/imgui/imgui.h"
	"${BL_LIB_DIR}/imgui/imgui.cpp"
	"${BL_LIB_DIR}/imgui/imgui_internal.h"
	"${BL_LIB_DIR}/imgui/imgui_demo.cpp"
	"${BL_LIB_DIR}/imgui/imgui_draw.cpp"
	"${BL_LIB_DIR}/imgui/imgui_tables.cpp"
	"${BL_LIB_DIR}/imgui/imgui_widgets.cpp"
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC "${BL_LIB_DIR}/imgui")


#--------------------------------------------------------------------------------------------------
#	Build
#--------------------------------------------------------------------------------------------------
add_executable(Blobolution ${BL_SRC})
target_include_directories(Blobolution PRIVATE ${BL_HSP})
target_precompile_headers(Blobolution PRIVATE "${BL_SRC_DIR}/Prefix.pch")
target_link_libraries(Blobolution PRIVATE glad glfw glm box2d imgui)

set_target_properties(Blobolution PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Blobolution>
)


#--------------------------------------------------------------------------------------------------
#	Resources
#--------------------------------------------------------------------------------------------------
add_custom_target(Blobolution_CopyResources ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${BL_RES_DIR} $<TARGET_FILE_DIR:Blobolution>
)