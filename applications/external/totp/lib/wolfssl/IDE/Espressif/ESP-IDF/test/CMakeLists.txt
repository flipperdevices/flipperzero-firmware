set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_MAIN_DRIVER -DWOLFSSL_USER_SETTINGS")

set(COMPONENT_SRCS "test_wolfssl.c" "../wolfcrypt/test/test.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES unity test_utils wolfssl)

register_component()
