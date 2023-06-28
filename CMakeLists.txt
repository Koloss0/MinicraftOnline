cmake_minimum_required(VERSION 3.13)

project(
	MCO
	DESCRIPTION "A remake of the classic game 'Minicraft' by Markus Persson (Notch) with added features and Multiplayer support."
	LANGUAGES C CXX
)

# specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# generate a compile commands JSON
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add warning flags
# other useful warnings: -Werror -Wno-unused -Wunreachable-code
set(c_flags -g)
set(cxx_flags -g -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wold-style-cast -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls -Wshadow -Wconversion -Wsign-conversion -Wsign-promo -Wsign-compare -Wstrict-aliasing -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wreturn-type -Wchar-subscripts -Wcomment -Wunused -Wno-unused-parameter -Weffc++)

add_subdirectory(engine engine/bin)
add_subdirectory(mco mco/bin)
