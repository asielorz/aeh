add_library(tests_main OBJECT
    main.tests.cc
    tests_main.hh
)
add_library(aeh::tests_main ALIAS tests_main)

target_link_libraries(tests_main
    PUBLIC
        Catch2::Catch2
    PRIVATE
        aeh
)

target_compile_features(tests_main
    PUBLIC
        cxx_std_17
)

set_target_properties(tests_main PROPERTIES
	UNITY_BUILD OFF # Single-file target
)
