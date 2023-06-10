cmake_minimum_required(VERSION 3.10)

project(MCO VERSION 0.1)

# generate a compile commands JSON
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in ${CMAKE_BINARY_DIR}/config.h)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/submodules/spdlog/include ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(MCO src/core/entry_point.cpp src/core/application.cpp src/game_layer.cpp src/mco_app.cpp src/core/layer_stack.cpp src/core/layer.cpp src/core/window.cpp src/core/log.cpp src/core/debug.cpp src/math/math.cpp src/ecs/scene.cpp src/ecs/systems/tilemap_system.cpp src/ecs/systems/player_system.cpp src/ecs/systems/sprite_animation_system.cpp src/ecs/systems/sprite_system.cpp src/io/image_loader.cpp src/glad.c src/renderer/renderer.cpp src/renderer/framebuffer.cpp src/renderer/shader.cpp src/renderer/material.cpp src/renderer/texture.cpp) # needs "${CURRENT_DIR}"?

target_link_libraries(MCO PUBLIC
	glfw3
	dl
	m
	GL
	X11
	pthread
	${EXTRA_LIBS}
)

target_include_directories(MCO PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${EXTRA_INCLUDES}"
)

# Other Useful Warnings:
# -Werror -Wno-unused -Wunreachable-code

set(c_flags -g)
set(cxx_flags -g -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wold-style-cast -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls -Wshadow -Wconversion -Wsign-conversion -Wsign-promo -Wsign-compare -Wstrict-aliasing -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wreturn-type -Wchar-subscripts -Wcomment -Wunused -Wno-unused-parameter -Weffc++)

target_compile_options(MCO PRIVATE "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(MCO PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
