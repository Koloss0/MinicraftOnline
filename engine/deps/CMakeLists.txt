target_include_directories(MCOEngine SYSTEM PUBLIC
	asio/include
	glad/include
	glm/include
	olcnet/include
	spdlog/include
	stb/include
)

add_subdirectory(glfw)
