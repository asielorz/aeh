import conans
import conans.tools
import conans.errors
import os.path

required_conan_version = ">=1.30.2" # The version I had when I first wrote the recipe

class AehConan(conans.ConanFile):
    name = "aeh"
    version = "0.6.0"
    license = "MIT"
    author = "Asier Elorz Hernáez (https://github.com/asielorz)"
    url = "https://github.com/asielorz/aeh" # Package repository, not library repository
    homepage = "https://github.com/asielorz/aeh"
    description = "C++ utilities library"
    topics = ("utilities")
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_find_package", "cmake_find_package_multi"
    options = {
        #"shared": [True, False], # Building as a DLL doesn't currently work
        "with_sdl2" : [True, False],
        "with_imgui" : [True, False],
        "with_glm" : [True, False],
        "with_unit_tests" : [True, False]
    } 
    default_options = {
        #"shared": False, # Building as a DLL doesn't currently work
        "with_sdl2" : True,
        "with_imgui" : True,
        "with_glm" : True,
        "with_unit_tests" : False,
    }

    def export_sources(self):
        self.copy("aeh/*")
        self.copy("tests/*")
        self.copy("CMakeLists.txt")
        self.copy("LICENSE.txt")

    def requirements(self):
        if self.options.with_unit_tests:
            self.requires("catch2/2.13.6")
        
        if self.options.with_glm:
            self.requires("glm/0.9.9.8")

        if self.options.with_imgui:
            if not self.options.with_sdl2:
                raise conans.errors.ConanInvalidConfiguration("ImGui requires SDL2")

            self.requires("imgui/1.79")

        if self.options.with_sdl2:
            self.requires("sdl2/2.0.14@bincrafters/stable")
            self.requires("opengl/system")

        self.requires("portable-file-dialogs/0.1.0")

    def configure(self):
        required_cpp_std = "20"
        if not conans.tools.valid_min_cppstd(self, required_cpp_std):
            raise conans.errors.ConanInvalidConfiguration("aeh requires C++" + required_cpp_std)

    def _configure_cmake(self):
        cmake = conans.CMake(self)
        cmake.definitions["AEH_BUILD_TESTS"] = self.options.with_unit_tests
        cmake.definitions["AEH_WITH_SDL2"] = self.options.with_sdl2
        cmake.definitions["AEH_WITH_IMGUI"] = self.options.with_imgui
        cmake.definitions["AEH_WITH_GLM"] = self.options.with_glm
        cmake.definitions["TREAT_WARNINGS_AS_ERRORS"] = self.options.with_unit_tests
        cmake.configure()
        
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["aeh"]
        
        self.cpp_info.includedirs = ["include", os.path.join("include", "aeh")]
        
        if self.options.with_glm:
            self.cpp_info.defines.extend(["AEH_WITH_GLM", "GLM_FORCE_SILENT_WARNINGS", "GLM_FORCE_EXPLICIT_CTOR"])
        
        if self.options.with_imgui:
            self.cpp_info.defines.append("AEH_WITH_IMGUI")
        
        if self.options.with_sdl2:
            self.cpp_info.defines.append("AEH_WITH_SDL2")

