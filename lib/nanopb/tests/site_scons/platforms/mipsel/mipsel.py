# Compiler settings for running the tests on mipsel-linux-gnu using
# qemu. Requires following packages to be installed:
# gcc-mipsel-linux-gnu g++-mipsel-linux-gnu qemu-user

def set_mipsel_platform(env):
    env.Replace(EMBEDDED = "MIPSEL")
    env.Replace(CC  = "mipsel-linux-gnu-gcc",
                CXX = "mipsel-linux-gnu-g++")
    env.Replace(TEST_RUNNER = "/usr/bin/qemu-mipsel")
    env.Append(LINKFLAGS = "-static")
    
