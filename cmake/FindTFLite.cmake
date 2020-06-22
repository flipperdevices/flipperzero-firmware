SET(LIB_NAME tensorflow-microlite)

IF(TF_RECACHE)
    MESSAGE(STATUS "Rebasing TensorFlow source")
    UNSET(TF_TAG CACHE)
    UNSET(TF_COMMIT CACHE)
ENDIF()

IF(NOT TF_SRC)
    INCLUDE(FetchContent)
    IF(TF_TAG)
        MESSAGE(STATUS "Getting TF tag '${TF_TAG}' and not master")
        FetchContent_Declare(
            tf 
            GIT_REPOSITORY https://github.com/tensorflow/tensorflow.git
            GIT_PROGRESS FALSE
            GIT_REMOTE_UPDATE_STRATEGY REBASE_CHECKOUT
            GIT_TAG ${TF_TAG}
            QUIET
            )
    ELSEIF(TF_COMMIT)
        MESSAGE(STATUS "Getting TF commit '${TF_COMMIT}' and not master")
        FetchContent_Declare(
            tf
            GIT_REPOSITORY https://github.com/tensorflow/tensorflow.git
            GIT_PROGRESS FALSE
            GIT_REMOTE_UPDATE_STRATEGY REBASE_CHECKOUT
            GIT_TAG ${TF_COMMIT}
            QUIET
            )
    ELSE()
        FetchContent_Declare(
            tf 
            GIT_REPOSITORY https://github.com/tensorflow/tensorflow.git
            GIT_PROGRESS FALSE
            GIT_REMOTE_UPDATE_STRATEGY REBASE_CHECKOUT
            QUIET
            )
    ENDIF()
    FetchContent_GetProperties(tf)
    IF(NOT tf_POPULATED)
        MESSAGE(STATUS "TensorFlow sources not given/populated, fetching from GH...")
        FetchContent_Populate(tf)
    ENDIF()
    SET(TF_SRC ${tf_SOURCE_DIR})

    FetchContent_Declare(
        flatbuffers 
        GIT_REPOSITORY https://github.com/google/flatbuffers.git 
        GIT_PROGRESS FALSE 
        QUIET
        )
    FetchContent_GetProperties(flatbuffers)
    IF(NOT flatbuffers_POPULATED)
        MESSAGE(STATUS "Now getting 'flatbuffers'...")
        FetchContent_Populate(flatbuffers)
    ENDIF()
    LIST(APPEND TFL_INC_DIRS ${flatbuffers_SOURCE_DIR}/include)

    FetchContent_Declare(
        fixedpoint 
        GIT_REPOSITORY https://github.com/google/gemmlowp.git 
        GIT_PROGRESS FALSE 
        QUIET 
        )
    FetchContent_GetProperties(fixedpoint)
    IF(NOT fixedpoint_POPULATED)
        MESSAGE(STATUS "And finaly 'fixedpoint'...")
        FetchContent_Populate(fixedpoint)
    ENDIF()
    LIST(APPEND TFL_INC_DIRS ${fixedpoint_SOURCE_DIR})

    FetchContent_Declare(
        ruy 
        GIT_REPOSITORY https://github.com/google/ruy.git 
        GIT_PROGRESS FALSE 
        QUIET 
        )
    FetchContent_GetProperties(ruy)
    IF(NOT ruy_POPULATED)
        MESSAGE(STATUS "Oh we also need 'ruy'...")
        FetchContent_Populate(ruy)
    ENDIF()
    LIST(APPEND TFL_INC_DIRS ${ruy_SOURCE_DIR})
ENDIF()

SET(TFL_SRC ${TF_SRC}/tensorflow/lite)
SET(TFLM_SRC ${TFL_SRC}/micro)
SET(TFLD_SRC ${TFL_SRC}/tools/make/downloads)

IF(EXISTS ${TFLD_SRC}/flatbuffers/include)
    LIST(APPEND TFL_INC_DIRS ${TFLD_SRC}/flatbuffers/include)
ENDIF()

IF(EXISTS ${TFLD_SRC}/gemmlowp)
    LIST(APPEND ${TFLD_SRC}/gemmlowp)
ENDIF()

LIST(APPEND TFL_INC_DIRS 
    ${TF_SRC}
    )

FILE(GLOB TFL_ROOT_SRCS
    ${TFLM_SRC}/*.cc 
    )

FILE(GLOB TFL_KERNELS_SRCS
    ${TFLM_SRC}/kernels/*.cc 
    ${TFL_SRC}/kernels/internal/quantization_util.cc 
    ${TFL_SRC}/kernels/kernel_util.cc
    )

FILE(GLOB TFL_CORE_API_SRCS
    ${TFL_SRC}/core/api/*.cc 
    )

FILE(GLOB TFL_C_SRCS
    ${TFL_SRC}/c/common.c
    )

FILE(GLOB TFL_MEM_PLANNER_SRCS
    ${TFLM_SRC}/memory_planner/*.cc
    )

SET(TFL_SRCS 
    ${TFL_ROOT_SRCS}
    ${TFL_KERNELS_SRCS}
    ${TFL_CORE_API_SRCS}
    ${TFL_C_SRCS}
    ${TFL_MEM_PLANNER_SRCS}
    )

LIST(FILTER TFL_SRCS EXCLUDE REGEX "([a-z0-9_]+_test.cc)$")

ADD_LIBRARY(${LIB_NAME} STATIC
    ${TFL_SRCS}
)

TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PUBLIC
    ${TFL_INC_DIRS}
)

TARGET_COMPILE_DEFINITIONS(${LIB_NAME} PUBLIC
    TF_LITE_USE_GLOBAL_MAX
    TF_LITE_USE_GLOBAL_MIN
    TF_LITE_USE_GLOBAL_CMATH_FUNCTIONS
    TF_LITE_STATIC_MEMORY
    TFLITE_EMULATE_FLOAT
    "$<$<CONFIG:RELEASE>:TF_LITE_STRIP_ERROR_STRINGS>"
)

SET(TFLite_INCLUDE_DIRS 
    ${TFL_INC_DIRS}
    )

SET(TFLite_SOURCES 
    ${TFL_SRCS}
    )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TFLite DEFAULT_MSG TFLite_INCLUDE_DIRS TFLite_SOURCES)
