cmake_minimum_required(VERSION 3.13)

project(MCOEngine VERSION 0.1)

add_library(MCOEngine src/core/entry_point.cpp src/core/application.cpp src/core/gui_application.cpp src/core/layer_stack.cpp src/core/layer.cpp src/core/window.cpp src/core/log.cpp src/core/debug.cpp src/math/math.cpp src/ecs/scene.cpp src/ecs/systems/tilemap_system.cpp src/ecs/systems/sprite_animation_system.cpp src/ecs/systems/sprite_system.cpp src/io/image_loader.cpp src/glad.c src/renderer/renderer.cpp src/renderer/framebuffer.cpp src/renderer/shader.cpp src/renderer/material.cpp src/renderer/texture.cpp)

configure_file(include/engine/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/engine/config.h)

add_subdirectory(deps)

target_include_directories(MCOEngine PUBLIC src include ${CMAKE_CURRENT_BINARY_DIR}/include)

find_package(OpenGL REQUIRED)

target_link_libraries(MCOEngine PRIVATE
	glfw3
	dl
	m
	GL
	X11
	pthread
)

target_link_directories(MCOEngine PRIVATE lib)

target_compile_options(MCOEngine PRIVATE "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(MCOEngine PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
