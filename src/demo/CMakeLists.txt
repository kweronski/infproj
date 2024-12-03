add_executable(demo_button
	demo_button.cpp
)

target_link_libraries(demo_button PRIVATE
	framework
)

set_target_properties(demo_button PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)
