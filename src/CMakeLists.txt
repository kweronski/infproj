add_compile_options(-Wall -Wextra -Wpedantic)

if (DEBUG)
	add_compile_options(-O0 -g)
	message(STATUS "Setup for debugging")
endif()

# Find the required dependencies
set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 COMPONENTS Graphics Network Audio)
find_package(PugiXML REQUIRED)

# Expand the search path for custom standard lib versions
set(CMAKE_INSTALL_RPATH "${CMAKE_BUILD_RPATH}:/usr/local/lib64")
set(CMAKE_BUILD_RPATH "${CMAKE_BUILD_RPATH}:/usr/local/lib64")

add_library(framework
	button_builder.cpp
	node_builder.cpp
	context.cpp
	builder.cpp
	scene.cpp
	node.cpp
)

target_link_libraries(framework PUBLIC
	SFML::Graphics
	SFML::Network
	SFML::Audio
	-lpugixml
)

add_executable(run main.cpp
	cfusion.cpp
	decrypt.cpp
	rps.cpp
	gomoku.cpp
)

set_target_properties(run PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)

target_link_libraries(run PRIVATE
	framework
)
