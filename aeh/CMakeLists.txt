set(ROOT_FILES
	src/align.cc
	src/align.hh
	src/batched_parallel_work.hh
	src/batched_parallel_work.inl
	src/binary_io.hh
	src/binary_io.inl
	src/change_stack.hh
	src/change_stack.inl
	src/circular.hh
	src/compatibility.hh
	src/expected.hh
	src/fixed_capacity_vector.hh
	src/fixed_capacity_vector.inl
	src/function_ptr.hh
	src/half.hh
	src/imgui_sdl_binding.cc
	src/imgui_sdl_binding.hh
	src/intcmp.hh
	src/multicomparison.hh
	src/out.hh
	src/overload.hh
	src/pointer_union.hh
	src/priority.hh
	src/randomly_seeded.hh
	src/span.hh
	src/string.cc
	src/string.hh
	src/string.inl
	src/tuple.hh
	src/tuple.inl
	src/unique_handle.hh
	src/unique_handle.inl
	src/value_ptr.hh
	src/warning_macro.hh
)

set(DEBUG_FILES
	src/debug/assert.cc
	src/debug/assert.hh
	src/debug/unreachable.cc
	src/debug/unreachable.hh
)

set(GL_FILES
	src/gl/gl_core_4_5.cc
	src/gl/gl_core_4_5.hh
)

set(MAIN_LOOP_FILES
	src/main_loop/demo_crtp_base.hh
	src/main_loop/demo_crtp_base.inl
	src/main_loop/imgui_plugin.cc
	src/main_loop/imgui_plugin.hh
	src/main_loop/input_plugin.cc
	src/main_loop/input_plugin.hh
	src/main_loop/main_loop.cc
	src/main_loop/main_loop.hh
	src/main_loop/optional_customization_points.hh
)

set(INPUT_FILES
	src/input/input.cc
	src/input/input.hh
	src/input/input_SDL.cc
	src/input/input_SDL.hh
	src/input/modifiers.cc
	src/input/modifiers.hh
	src/input/mouse_ndc.cc
	src/input/mouse_ndc.hh
	src/input/repeat_controller.cc
	src/input/repeat_controller.hh
	src/input/xm_controller_axes.hh
	src/input/xm_controller_buttons.hh
	src/input/xm_keys.hh
	src/input/xm_mouse_buttons.hh
)

set(MSP_FILES
	src/msp/copy_on_write.hh
	src/msp/copy_on_write.inl
	src/msp/flexible_immutable_array.hh
	src/msp/flexible_immutable_array.inl
	src/msp/minimalistic_shared_ptr.hh
	src/msp/minimalistic_shared_ptr.inl
)

add_library(aeh_lib OBJECT
	${ROOT_FILES}
	${DEBUG_FILES}
	${GL_FILES}
	${MAIN_LOOP_FILES}
	${INPUT_FILES}
	${MSP_FILES}
	
	aeh.natvis
)

source_group("source" FILES	${ROOT_FILES})
source_group("debug" FILES	${DEBUG_FILES})
source_group("gl" FILES	${GL_FILES})
source_group("main_loop" FILES	${MAIN_LOOP_FILES})
source_group("input" FILES	${INPUT_FILES})
source_group("msp" FILES	${MSP_FILES})

target_compile_features(aeh_lib
	PUBLIC
		cxx_std_17
)

target_link_libraries(aeh_lib
	PRIVATE
		opengl32
		CONAN_PKG::sdl2
		CONAN_PKG::imgui
)

target_compile_definitions(aeh_lib
	PUBLIC
		NOMINMAX # For Windows.h not to defined 'min' and 'max' macros
		AEH_HAS_IMGUI
		AEH_HAS_SDL
	PRIVATE
		$<$<CONFIG:Debug>:AEH_DEBUG>
		AEH_BUILD_TYPE=$<CONFIG>
)

if (MSVC)
	target_compile_options(aeh_lib
		PRIVATE
			/bigobj
			/MP # Multi-core compilation
  	)
endif()

if (UNIX)
	target_link_libraries(aeh_lib
		PRIVATE
			${CMAKE_DL_LIBS} # For 'dlopen' and related
	)
endif()
