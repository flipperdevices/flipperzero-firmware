INCLUDE(FetchContent)

SET(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
    arm_cmsis
    GIT_REPOSITORY https://github.com/ARM-software/CMSIS_5.git
    )

FetchContent_GetProperties(arm_cmsis)
IF(NOT arm_cmsis_POPULATED)
    MESSAGE(STATUS "Getting most recent ARM CMSIS sources")
    FetchContent_Populate(arm_cmsis)
    EXECUTE_PROCESS(COMMAND git -C ${arm_cmsis_SOURCE_DIR} checkout develop)
ENDIF()

# FIND_PACKAGE(Fixedpoint)

SET(ARM_CMSIS_DIR ${arm_cmsis_SOURCE_DIR}/CMSIS)

SET(CMSIS_NN_HEADERS
    arm_nnfunctions.h
    arm_nnsupportfunctions.h
    arm_nn_tables.h
    )

FILE(GLOB CMSIS_NN_SOURCES ${ARM_CMSIS_DIR}/NN/Source/*/*.c)

FIND_PATH(CMSIS_NN_INC_DIR ${CMSIS_NN_HEADERS}
    PATHS ${ARM_CMSIS_DIR}/NN/Include
    CMAKE_FIND_ROOT_PATH_BOTH
    )

SET(CMSIS_DSP_HEADERS
    arm_common_tables.h
    arm_const_structs.h
    arm_helium_utils.h
    arm_math.h
    arm_mve_tables.h
    arm_vec_math.h
    )

SET(CMSIS_DSP_PRIVATE_HEADERS
    arm_sorting.h
    arm_vec_fft.h
    arm_vec_filtering.h
    )

SET(CMSIS_DSP_SRCS
    BasicMathFunctions.c
    arm_gaussian_naive_bayes_predict_f32.c
    CommonTables.c
    ComplexMathFunctions.c
    ControllerFunctions.c
    arm_boolean_distance.c
    arm_boolean_distance_template.h
    arm_braycurtis_distance_f32.c
    arm_canberra_distance_f32.c
    arm_chebyshev_distance_f32.c
    arm_cityblock_distance_f32.c
    arm_correlation_distance_f32.c
    arm_cosine_distance_f32.c
    arm_dice_distance.c
    arm_euclidean_distance_f32.c
    arm_hamming_distance.c
    arm_jaccard_distance.c
    arm_jensenshannon_distance_f32.c
    arm_kulsinski_distance.c
    arm_minkowski_distance_f32.c
    arm_rogerstanimoto_distance.c
    arm_russellrao_distance.c
    arm_sokalmichener_distance.c
    arm_sokalsneath_distance.c
    arm_yule_distance.c
    FastMathFunctions.c
    FilteringFunctions.c
    MatrixFunctions.c
    StatisticsFunctions.c
    SupportFunctions.c
    arm_svm_linear_init_f32.c
    arm_svm_linear_predict_f32.c
    arm_svm_polynomial_init_f32.c
    arm_svm_polynomial_predict_f32.c
    arm_svm_rbf_init_f32.c
    arm_svm_rbf_predict_f32.c
    arm_svm_sigmoid_init_f32.c
    arm_svm_sigmoid_predict_f32.c
    TransformFunctions.c
    )

FOREACH(SRC ${CMSIS_DSP_SRCS})
    STRING(MAKE_C_IDENTIFIER "${SRC}" SRC_CLEAN)
    SET(CMSIS_DSP_${SRC_CLEAN}_FILE ${SRC_CLEAN}-NOTFOUND)
    FIND_FILE(CMSIS_DSP_${SRC_CLEAN}_FILE ${SRC}
        PATH_SUFFIXES
        BasicMathFunctions
        BayesFunctions
        CommonTables
        ComplexMathFunctions
        ControllerFunctions
        DistanceFunctions
        FastMathFunctions
        FilteringFunctions
        MatrixFunctions
        StatisticsFunctions
        SupportFunctions
        SVMFunctions
        TransformFunctions
        PATHS ${ARM_CMSIS_DIR}/DSP/Source
        CMAKE_FIND_ROOT_PATH_BOTH
        )
    LIST(APPEND CMSIS_DSP_SOURCES ${CMSIS_DSP_${SRC_CLEAN}_FILE})
ENDFOREACH()

FIND_PATH(CMSIS_DSP_PRIVATE_INC_DIR ${CMSIS_DSP_PRIVATE_HEADERS}
    PATHS ${ARM_CMSIS_DIR}/DSP/PrivateInclude
    CMAKE_FIND_ROOT_PATH_BOTH
    )

FIND_PATH(CMSIS_DSP_INC_DIR ${CMSIS_DSP_HEADERS}
    PATHS ${ARM_CMSIS_DIR}/DSP/Include
    CMAKE_FIND_ROOT_PATH_BOTH
    )

SET(CMSIS_DSP_INC_DIRS
    ${CMSIS_DSP_PRIVATE_INC_DIR}
    ${CMSIS_DSP_INC_DIR}
    )

SET(CMSISNN_INCLUDE_DIRS
    ${CMSIS_DSP_INC_DIRS}
    ${CMSIS_NN_INC_DIR}
    )

SET(CMSISNN_SOURCES
    ${CMSIS_DSP_SOURCES}
    ${CMSIS_NN_SOURCES}
    )

IF(STM32_FAMILY STREQUAL "F0")
    ADD_DEFINITIONS(-DARM_MATH_CM0)
ELSEIF(STM32_FAMILY STREQUAL "F3")
    ADD_DEFINITIONS(-DARM_MATH_CM3)
ELSEIF(STM32_FAMILY STREQUAL "F4")
    #TODO find better solution to this
    ADD_DEFINITIONS(-D__FPU_PRESENT=1)
    ADD_DEFINITIONS(-DARM_MATH_CM4)
ELSEIF(STM32_FAMILY STREQUAL "F7")
    ADD_DEFINITIONS(-DARM_MATH_CM7)
ELSEIF(STM32_FAMILY STREQUAL "L0")
    ADD_DEFINITIONS(-DARM_MATH_CM0PLUS)
ELSE()
    MESSAGE(STATUS "ARM_MATH define not found, see arm_math.h")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CMSISNN DEFAULT_MSG CMSISNN_INCLUDE_DIRS CMSISNN_SOURCES)
