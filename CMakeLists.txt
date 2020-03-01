cmake_minimum_required(VERSION 3.9)

project(console-markdown LANGUAGES C CXX VERSION 1.0 DESCRIPTION "Render markdown in the console")

if(MSVC)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
	set(CMAKE_CXX_FLAGS "-std=gnu++17")
endif()

add_library(console-markdown STATIC
	ColouredBuffer.cpp
	ColouredBuffer.hpp
	StringUtils.cpp
	StringUtils.hpp
	ConsoleRenderer.cpp
	console-colour.h
	console-colour.c
	parsers/pawn.cpp
	parsers/cpp.cpp
	include/console-markdown/CPP.hpp
	include/console-markdown/Pawn.hpp
	include/console-markdown/CMDMD.hpp
	libsoldout/array.c
	libsoldout/array.h
	libsoldout/buffer.c
	libsoldout/buffer.h
	libsoldout/markdown.c
	libsoldout/markdown.h
)

set_target_properties(console-markdown PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(console-markdown PROPERTIES SOVERSION 1)

if(MSVC)
	# Include <strings.h> in Windows from our sources.
	target_include_directories(console-markdown PRIVATE .)
endif()

add_executable(console-markdown-test main.cpp)
target_link_libraries(console-markdown-test console-markdown)

target_include_directories(console-markdown-test PRIVATE include)

