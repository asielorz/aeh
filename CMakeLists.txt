cmake_minimum_required(VERSION 3.13)

get_directory_property(is_subproject PARENT_DIRECTORY)
if(is_subproject)
	message(STATUS "aeh: Building as subproject")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/submodules/build-utils/cmake")
if (NOT is_subproject)
	include(static-msvc-runtime)
	set_old_msvc_runtime_policy()
endif()

project(aeh CXX)

if (NOT is_subproject)
	set_static_msvc_runtime()
	include(set-globals)
	include(set-link-time-optimization)
endif()

option(AEH_WITH_SDL2 "Build components depending on SDL2" ON)
option(AEH_WITH_IMGUI "Build components depending on Dear ImGui" ON)
option(AEH_WITH_GLM "Build components depending on GLM" ON)
option(AEH_BUILD_TESTS "Build unit-tests" ON)
if (AEH_WITH_IMGUI AND NOT AEH_WITH_IMGUI)
	message(FATAL_ERROR "Dear ImGui depends on SDL2")
endif()

if (CONAN_EXPORTED) # Running from conan, building a package
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
else() # Development, running directly from cmake
	include(conan-run)
	if (NOT is_subproject)
		set(CMAKE_CONFIGURATION_TYPES Debug Release)
	endif()

	set(conan_options)
	add_conan_bool_option_to_list(conan_options ${AEH_WITH_SDL2} "with_sdl2")
	add_conan_bool_option_to_list(conan_options ${AEH_WITH_IMGUI} "with_imgui")
	add_conan_bool_option_to_list(conan_options ${AEH_WITH_GLM} "with_glm")
	add_conan_bool_option_to_list(conan_options ${AEH_BUILD_TESTS} "with_unit_tests")

	conan_run(
		OPTIONS
			${conan_options}
		EXTRA_SETTINGS
			"compiler.cppstd=20"
	)
endif()

if (NOT is_subproject)
	option(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" TRUE)
	if (TREAT_WARNINGS_AS_ERRORS)
		include(warnings-as-errors)
		add_warnings_as_errors()
	endif()

	include(common-flags)
	add_common_flags()
endif()

install(FILES "LICENSE.txt" DESTINATION ".")
add_subdirectory(aeh)

if (AEH_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
