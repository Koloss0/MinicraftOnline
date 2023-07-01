configure_file(config.h.in include/base/config.h)

add_executable(Base game_layer.cpp mco_app.cpp tiles.cpp systems/player_system.cpp)
target_include_directories(Base PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(Base PRIVATE Engine)

target_compile_options(Base PRIVATE "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(Base PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")

option(COMPILE_SERVER "Build a headless server" OFF)
if(COMPILE_SERVER)
	message("Server build enabled")
	target_compile_definitions(Base PRIVATE COMPILE_SERVER)
endif(COMPILE_SERVER)
