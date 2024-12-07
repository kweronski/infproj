add_executable(demo_button
	demo_button.cpp
)

target_link_libraries(demo_button PRIVATE
	framework
)

set_target_properties(demo_button PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)

add_executable(demo_builder
	demo_builder.cpp
)

target_link_libraries(demo_builder PRIVATE
	framework
)

set_target_properties(demo_builder PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)

add_executable(demo_context
	demo_context.cpp
)

target_link_libraries(demo_context PRIVATE
	framework
)

set_target_properties(demo_context PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)

add_executable(demo_registers
	demo_registers.cpp
)

target_link_libraries(demo_registers PRIVATE
	framework
)

set_target_properties(demo_registers PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${exedir}
)
