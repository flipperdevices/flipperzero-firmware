# Compiler settings for running the tests on riscv64-linux-gnu
# using qemu. Requires following packages to be installed:
# gcc-riscv64-linux-gnu g++-riscv64-linux-gnu qemu-user

def set_riscv64_platform(env):
    env.Replace(EMBEDDED = "RISCV64")
    env.Replace(CC  = "riscv64-linux-gnu-gcc",
                CXX = "riscv64-linux-gnu-g++")
    env.Replace(TEST_RUNNER = "/usr/bin/qemu-riscv64")
    env.Append(LINKFLAGS = "-static")
    
