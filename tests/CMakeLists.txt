find_package(Catch2 REQUIRED)
add_subdirectory(tests_main)

add_executable(aeh_tests
	src/algorithm.tests.cc
	src/batched_parallel_work.tests.cc
	src/fixed_capacity_vector.tests.cc
	src/function_ref.tests.cc
	src/half.tests.cc
	src/input.tests.cc
	src/pointer_union.tests.cc
	src/span.tests.cc
	src/string.tests.cc
)
add_executable(aeh::aeh_tests ALIAS aeh_tests)

target_include_directories(aeh
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/aeh/src/"
)

target_link_libraries(aeh_tests
	PRIVATE
		aeh
		tests_main
		Catch2::Catch2
)

target_compile_definitions(aeh_tests
	PRIVATE
		$<$<CONFIG:Debug>:AEH_DEBUG>
		AEH_BUILD_TYPE=$<CONFIG>
)

if (MSVC)
	target_compile_options(aeh_tests
		PRIVATE
			/MP # Multi-core compilation
  	)
endif()

add_test(
	NAME unit_tests
	COMMAND aeh_tests
)
