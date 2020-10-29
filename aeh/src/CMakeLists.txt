set(ROOT_FILES
	align.cc
	align.hh
	batched_parallel_work.hh
	batched_parallel_work.inl
	binary_io.hh
	binary_io.inl
	change_stack.hh
	change_stack.inl
	circular.hh
	compatibility.hh
	expected.hh
	fixed_capacity_vector.hh
	fixed_capacity_vector.inl
	function_ptr.hh
	function_ref.hh
	function_ref.inl
	half.hh
	imgui_sdl_binding.cc
	imgui_sdl_binding.hh
	intcmp.hh
	multicomparison.hh
	out.hh
	overload.hh
	pointer_union.hh
	priority.hh
	randomly_seeded.hh
	span.hh
	string.cc
	string.hh
	string.inl
	tuple.hh
	tuple.inl
	unique_handle.hh
	unique_handle.inl
	value_ptr.hh
	warning_macro.hh
)
target_sources(aeh PRIVATE ${ROOT_FILES})
source_group("source" FILES	${ROOT_FILES})

set(DEBUG_FILES
	debug/assert.cc
	debug/assert.hh
	debug/unreachable.cc
	debug/unreachable.hh
)
target_sources(aeh PRIVATE ${DEBUG_FILES})
source_group("debug" FILES ${DEBUG_FILES})

set(MAIN_LOOP_FILES
	main_loop/demo_crtp_base.hh
	main_loop/demo_crtp_base.inl
	main_loop/imgui_plugin.cc
	main_loop/imgui_plugin.hh
	main_loop/input_plugin.cc
	main_loop/input_plugin.hh
	main_loop/main_loop.cc
	main_loop/main_loop.hh
	main_loop/main_loop.inl
	main_loop/optional_customization_points.hh
)
target_sources(aeh PRIVATE ${MAIN_LOOP_FILES})
source_group("main_loop" FILES ${MAIN_LOOP_FILES})

if (AEH_WITH_SDL2)
    set(GL_FILES
	    gl/gl_core_4_5.cc
        gl/gl_core_4_5.hh
    )
    target_sources(aeh PRIVATE ${GL_FILES})
    source_group("gl" FILES	${GL_FILES})
endif()

set(INPUT_FILES
	input/input.cc
	input/input.hh
	input/input_SDL.cc
	input/input_SDL.hh
	input/modifiers.cc
	input/modifiers.hh
	input/mouse_ndc.cc
	input/mouse_ndc.hh
	input/repeat_controller.cc
	input/repeat_controller.hh
	input/xm_controller_axes.hh
	input/xm_controller_buttons.hh
	input/xm_keys.hh
	input/xm_mouse_buttons.hh
)
target_sources(aeh PRIVATE ${INPUT_FILES})
source_group("input" FILES ${INPUT_FILES})

set(MSP_FILES
	msp/copy_on_write.hh
	msp/copy_on_write.inl
	msp/flexible_immutable_array.hh
	msp/flexible_immutable_array.inl
	msp/minimalistic_shared_ptr.hh
	msp/minimalistic_shared_ptr.inl
)
target_sources(aeh PRIVATE ${MSP_FILES})
source_group("msp" FILES ${MSP_FILES})

