import conans
import os

class AehTestConan(conans.ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_multi"

    def build(self):
        cmake = conans.CMake(self)
        # Current dir is "test_package/build/<build_id>" and CMakeLists.txt is
        # in "test_package"
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self):
        if not conans.tools.cross_building(self):
            with conans.tools.chdir("bin"):
                self.run(".{}example".format(os.sep))
