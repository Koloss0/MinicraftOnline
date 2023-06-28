cmake_minimum_required(VERSION 3.13)

project(MCOBase VERSION 0.1)

# add option to compile server
option(COMPILE_SERVER "Compiles a headless server" OFF)

add_executable(MCOBase src/game_layer.cpp src/mco_app.cpp src/tiles.cpp src/systems/player_system.cpp)

if(COMPILE_SERVER)
	message("compiling server...")
	target_compile_definitions(MCOBase PRIVATE COMPILE_SERVER)
endif(COMPILE_SERVER)

message("current bin dir: ${CMAKE_CURRENT_BINARY_DIR}")
message("project bin dir: ${PROJECT_BINARY_DIR}")
configure_file(include/mco/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/mco/config.h)

target_include_directories(MCOBase PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(MCOBase PRIVATE
	MCOEngine
)

target_compile_options(MCOBase PRIVATE "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(MCOBase PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
