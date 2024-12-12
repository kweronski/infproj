add_compile_options(-Wall -Wextra -Wpedantic)

if (DEBUG)
	add_compile_options(-O0 -g)
	message(STATUS "Setup for debugging")
endif()

# Find the required dependencies
find_package(SFML COMPONENTS system graphics network audio window REQUIRED)
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
	-lsfml-system
	-lsfml-window
	-lsfml-graphics
	-lsfml-network
	-lsfml-audio
	-lpugixml
)

add_executable(run main.cpp
)

set_target_properties(run PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)

target_link_libraries(run PRIVATE
	framework
)
