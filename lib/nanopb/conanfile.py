from conans import ConanFile, CMake, tools
from os import path

class NanoPbConan(ConanFile):
    name = "nanopb"
    version = "0.4.5"
    license = "zlib"
    url = "https://jpa.kapsi.fi/nanopb/"
    description = "Protocol Buffers with small code size"
    settings = "os_build", "compiler", "build_type", "arch"
    generators = "cmake"
    exports = '*'
    options = {
        "fPIC": [True, False],
    }
    default_options = {
        "fPIC": True,
    }

    def configure(self):
        if self.settings.os_build == "Windows" and self.settings.compiler == "Visual Studio":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=path.join(self.source_folder, "conan-wrapper"))
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.libs = ["protobuf-nanopb"]
