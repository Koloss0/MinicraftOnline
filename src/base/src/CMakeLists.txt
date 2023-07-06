configure_file(config.hpp.in include/base/config.hpp)

add_library(Base OBJECT base_game_layer.cpp tiles.cpp systems/player_system.cpp)
target_include_directories(Base PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(Base PUBLIC Engine)

target_compile_options(Base PUBLIC "$<$<COMPILE_LANGUAGE:C>:${c_flags}>")
target_compile_options(Base PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
