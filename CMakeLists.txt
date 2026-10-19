cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(TickAssist VERSION 0.1.0)

add_library(${PROJECT_NAME} SHARED src/main.cpp)

if (NOT DEFINED ENV{GEODE_SDK})
	message(FATAL_ERROR "Define GEODE_SDK")
endif()
add_compile_definitions(FMT_USE_CONSTEVAL=0)
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
setup_geode_mod(${PROJECT_NAME})
