configure_file(config.hpp.in include/engine/config.hpp)

add_library(Engine "")
add_subdirectory(core)
add_subdirectory(ecs)
add_subdirectory(io)
add_subdirectory(math)
add_subdirectory(renderer)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(Engine PRIVATE
	dl
	X11
	GL
	pthread
)

target_compile_options(Engine PRIVATE "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(Engine PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
