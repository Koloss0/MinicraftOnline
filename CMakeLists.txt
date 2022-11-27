cmake_minimum_required(VERSION 3.10)

project(MCO VERSION 0.1)

# specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(include/config.h.in config.h)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(MCO src/MCO/main.cpp src/glad/glad.c src/MCO/renderer.cpp src/MCO/shader.cpp src/MCO/material.cpp) # needs "${CURRENT_DIR}"?

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

target_compile_options(MCO PRIVATE -g)
