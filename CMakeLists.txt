cmake_minimum_required(VERSION 4.3)
project(number-stickmatches
	VERSION 0.0.1
	LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
	              EXPORT_COMPILE_COMMANDS ON)
