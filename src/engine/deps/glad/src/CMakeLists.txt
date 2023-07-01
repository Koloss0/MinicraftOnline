configure_file(config.h.in include/engine/config.h)

add_library(Engine core/entry_point.cpp core/application.cpp core/gui_application.cpp core/layer_stack.cpp core/layer.cpp core/window.cpp core/log.cpp core/debug.cpp math/math.cpp ecs/scene.cpp ecs/systems/tilemap_system.cpp ecs/systems/sprite_animation_system.cpp ecs/systems/sprite_system.cpp io/image_loader.cpp renderer/renderer.cpp renderer/framebuffer.cpp renderer/shader.cpp renderer/material.cpp renderer/texture.cpp)

target_include_directories(Engine
	PRIVATE src
	PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(Engine PRIVATE
	glfw
	dl
	m
	GL
	X11
	pthread
)

target_compile_options(Engine PRIVATE "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(Engine PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
