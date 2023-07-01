add_subdirectory(glad)
add_subdirectory(glfw)

target_include_directories(Engine SYSTEM PUBLIC
	asio/include
	glm/include
	olcnet/include
	spdlog/include
	stb/include
)

target_link_libraries(Engine PUBLIC
	glfw
	glad
)
