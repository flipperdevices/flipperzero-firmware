# Compiler settings for running the tests on mips-linux-gnu (big endian)
# using qemu. Requires following packages to be installed:
# gcc-mips-linux-gnu g++-mips-linux-gnu qemu-user

def set_mips_platform(env):
    env.Replace(EMBEDDED = "MIPS")
    env.Replace(CC  = "mips-linux-gnu-gcc",
                CXX = "mips-linux-gnu-g++")
    env.Replace(TEST_RUNNER = "/usr/bin/qemu-mips")
    env.Append(LINKFLAGS = "-static")
    
