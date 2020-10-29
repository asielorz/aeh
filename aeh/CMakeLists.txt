add_library(aeh)
add_library(aeh::aeh ALIAS aeh)

if (MSVC)
	target_sources(aeh PRIVATE aeh.natvis)
endif()

target_include_directories(aeh
	PUBLIC  
    	$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if (AEH_WITH_SDL2)
	find_package(SDL2 REQUIRED)
	target_link_libraries(aeh PUBLIC SDL2::SDL2)
	target_compile_definitions(aeh PUBLIC AEH_WITH_SDL2)
endif()

if (AEH_WITH_IMGUI)
	find_package(imgui REQUIRED)
	target_link_libraries(aeh PRIVATE imgui::imgui)
	target_compile_definitions(aeh PUBLIC AEH_WITH_IMGUI)
endif()

target_compile_features(aeh
	PUBLIC
		cxx_std_17
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

add_subdirectory(src)

install(TARGETS aeh)
install(DIRECTORY src/
		DESTINATION include/aeh
		FILES_MATCHING
			PATTERN "*.hh"
			PATTERN "*.inl"
)
install(FILES aeh.natvis DESTINATION ".")
