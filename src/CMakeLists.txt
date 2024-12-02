# Find the required dependencies
find_package(SFML COMPONENTS system graphics network audio window REQUIRED)
find_package(PugiXML REQUIRED)

# Expand the search path for custom standard lib versions
set(CMAKE_INSTALL_RPATH "${CMAKE_BUILD_RPATH}:/usr/local/lib64")
set(CMAKE_BUILD_RPATH "${CMAKE_BUILD_RPATH}:/usr/local/lib64")

add_executable(run main.cpp)

set_target_properties(run PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(run PRIVATE
	-lsfml-system
	-lsfml-window
	-lsfml-graphics
	-lsfml-network
	-lsfml-audio
	-lpugixml
)
