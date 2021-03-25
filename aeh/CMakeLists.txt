add_library(aeh)
add_library(aeh::aeh ALIAS aeh)

if (MSVC)
	target_sources(aeh PRIVATE aeh.natvis)
endif()

target_include_directories(aeh
	PUBLIC  
    	$<INSTALL_INTERFACE:include>
    	$<INSTALL_INTERFACE:include/aeh>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if (AEH_WITH_SDL2)
	find_package(SDL2 REQUIRED)
	target_link_libraries(aeh PUBLIC sdl2::sdl2)
	target_compile_definitions(aeh PUBLIC AEH_WITH_SDL2)
	
	find_package(OpenGL REQUIRED)
	target_link_libraries(aeh PUBLIC OpenGL::GL)
endif()

if (AEH_WITH_IMGUI)
	find_package(imgui REQUIRED)
	target_link_libraries(aeh PUBLIC imgui::imgui)
	target_compile_definitions(aeh PUBLIC AEH_WITH_IMGUI)
endif()

if (AEH_WITH_GLM)
	find_package(glm REQUIRED)
	target_link_libraries(aeh PUBLIC glm::glm)
	target_compile_definitions(aeh PUBLIC AEH_WITH_GLM
		# https://github.com/g-truc/glm/blob/master/manual.md
		GLM_FORCE_SILENT_WARNINGS # Silence warnings for nameless struct/union, etc
		GLM_FORCE_EXPLICIT_CTOR # Make make conversion constructors explicit; vec4 foo = ...; vec3 bar = foo; // Now it fails to compile
	)
endif()

target_compile_features(aeh
	PUBLIC
		cxx_std_20
)

target_compile_definitions(aeh
	PUBLIC
		NOMINMAX # For Windows.h not to defined 'min' and 'max' macros
	PRIVATE
		$<$<CONFIG:Debug>:AEH_DEBUG>
		AEH_BUILD_TYPE=$<CONFIG>
)

if (MSVC)
	target_compile_options(aeh
		PRIVATE
			/bigobj
			/MP # Multi-core compilation
  	)
endif()

target_sources(aeh PRIVATE
	src/algorithm.hh	
	src/algorithm.inl
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
	src/file_vector.cc
	src/file_vector.hh
	src/file_vector.inl
	src/fixed_capacity_vector.hh
	src/fixed_capacity_vector.inl
	src/function_ptr.hh
	src/function_ref.hh
	src/function_ref.inl
	src/half.hh
	src/imgui_sdl_binding.cc
	src/imgui_sdl_binding.hh
	src/intcmp.hh
	src/map.hh
	src/math.cc
	src/math.hh
	src/monadic_optional.hh
	src/multicomparison.hh
	src/out.hh
	src/overload.hh
	src/pointer_union.hh
	src/priority.hh
	src/randomly_seeded.hh
	src/range_function.hh
	src/span.hh
	src/string.cc
	src/string.hh
	src/string.inl
	src/tuple.hh
	src/tuple.inl
	src/unique_handle.hh
	src/unique_handle.inl
	src/value_ptr.hh
	src/viewport.cc
	src/viewport.hh
	src/warning_macro.hh

	src/debug/assert.cc
	src/debug/assert.hh
	src/debug/unreachable.cc
	src/debug/unreachable.hh

	src/main_loop/demo_crtp_base.hh
	src/main_loop/demo_crtp_base.inl
	src/main_loop/imgui_plugin.cc
	src/main_loop/imgui_plugin.hh
	src/main_loop/input_plugin.cc
	src/main_loop/input_plugin.hh
	src/main_loop/main_loop.cc
	src/main_loop/main_loop.hh
	src/main_loop/main_loop.inl
	src/main_loop/optional_customization_points.hh

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

	src/msp/copy_on_write.hh
	src/msp/copy_on_write.inl
	src/msp/flexible_immutable_array.hh
	src/msp/flexible_immutable_array.inl
	src/msp/minimalistic_shared_ptr.hh
	src/msp/minimalistic_shared_ptr.inl
)
if (AEH_WITH_SDL2)
	target_sources(aeh PRIVATE
		src/gl/gl_core_4_5.cc
		src/gl/gl_core_4_5.hh
	)
endif()

file(GLOB_RECURSE all_sources
	"*.hh" "*.cc" "*.inl"
)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
	FILES ${all_sources}
)

install(TARGETS aeh)
install(DIRECTORY src/
		DESTINATION include/aeh
		FILES_MATCHING
			PATTERN "*.hh"
			PATTERN "*.inl"
)
install(FILES aeh.natvis DESTINATION ".")
