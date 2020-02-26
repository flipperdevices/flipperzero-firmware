IF(NOT STM32Cube_DIR)
    SET(STM32Cube_DIR "/opt/STM32Cube_FW_F1_V1.2.0")
    MESSAGE(STATUS "No STM32Cube_DIR specified, using default: " ${STM32Cube_DIR})
ENDIF()

SET(USBDevice_SRC
    Core/Src/usbd_ctlreq.c
    Core/Src/usbd_core.c
    Core/Src/usbd_ioreq.c
#    Core/Src/usbd_conf_template.c
)

SET(USBDevice_INC
    Core/Inc/usbd_ctlreq.h
    Core/Inc/usbd_ioreq.h
    Core/Inc/usbd_core.h
#    Core/Inc/usbd_conf_template.h
    Core/Inc/usbd_def.h
)

SET(USBDevice_COMPONENTS CDC DFU AUDIO Template MSC HID CustomHID)

SET(USBDevice_COMPONENTS_CDC_HEADERS
    Class/CDC/Inc/usbd_cdc.h
#    Class/CDC/Inc/usbd_cdc_if_template.h
)
SET(USBDevice_COMPONENTS_CDC_SOURCES
    Class/CDC/Src/usbd_cdc.c
#    Class/CDC/Src/usbd_cdc_if_template.c
)

SET(USBDevice_COMPONENTS_DFU_HEADERS
#    Class/DFU/Inc/usbd_dfu_media_template.h
    Class/DFU/Inc/usbd_dfu.h
)
SET(USBDevice_COMPONENTS_DFU_SOURCES
    Class/DFU/Src/usbd_dfu.c
#    Class/DFU/Src/usbd_dfu_media_template.c
)

SET(USBDevice_COMPONENTS_AUDIO_HEADERS
    Class/AUDIO/Inc/usbd_audio.h
#    Class/AUDIO/Inc/usbd_audio_if_template.h
)
SET(USBDevice_COMPONENTS_AUDIO_SOURCES
#    Class/AUDIO/Src/usbd_audio_if_template.c
    Class/AUDIO/Src/usbd_audio.c
)

SET(USBDevice_COMPONENTS_Template_HEADERS
    Class/Template/Inc/usbd_template.h
) 
SET(USBDevice_COMPONENTS_Template_SOURCES
    Class/Template/Src/usbd_template.c
)

SET(USBDevice_COMPONENTS_MSC_HEADERS
    Class/MSC/Inc/usbd_msc_scsi.h
#    Class/MSC/Inc/usbd_msc_storage_template.h
    Class/MSC/Inc/usbd_msc_data.h
    Class/MSC/Inc/usbd_msc.h
    Class/MSC/Inc/usbd_msc_bot.h
)
SET(USBDevice_COMPONENTS_MSC_SOURCES
    Class/MSC/Src/usbd_msc.c
    Class/MSC/Src/usbd_msc_data.c
    Class/MSC/Src/usbd_msc_bot.c
    Class/MSC/Src/usbd_msc_scsi.c
#    Class/MSC/Src/usbd_msc_storage_template.c
)

SET(USBDevice_COMPONENTS_HID_HEADERS
    Class/HID/Inc/usbd_hid.h
)
SET(USBDevice_COMPONENTS_HID_SOURCES
    Class/HID/Src/usbd_hid.c
)

SET(USBDevice_COMPONENTS_CustomHID_HEADERS
#    Class/CustomHID/Inc/usbd_customhid_if_template.h
    Class/CustomHID/Inc/usbd_customhid.h
)
SET(USBDevice_COMPONENTS_CustomHID_SOURCES
    Class/CustomHID/Src/usbd_customhid.c
#    Class/CustomHID/Src/usbd_customhid_if_template.c
)

IF(NOT USBDevice_FIND_COMPONENTS)
    SET(USBDevice_FIND_COMPONENTS ${USBDevice_COMPONENTS})
    MESSAGE(STATUS "No USBDevice components selected, using all: ${USBDevice_FIND_COMPONENTS}")
ENDIF()

FOREACH(cmp ${USBDevice_FIND_COMPONENTS})
    LIST(FIND USBDevice_COMPONENTS ${cmp} USBDevice_FOUND_INDEX)
    IF(${USBDevice_FOUND_INDEX} LESS 0)
        MESSAGE(FATAL_ERROR "Unknown USBDevice component: ${cmp}. Available components: ${USBDevice_COMPONENTS}")
    ENDIF()
    LIST(FIND USBDevice_COMPONENTS ${cmp} USBDevice_FOUND_INDEX)
    IF(NOT (${USBDevice_FOUND_INDEX} LESS 0))
        LIST(APPEND USBDevice_INC ${USBDevice_COMPONENTS_${cmp}_HEADERS})
        LIST(APPEND USBDevice_SRC ${USBDevice_COMPONENTS_${cmp}_SOURCES})
    ENDIF()
ENDFOREACH()

LIST(REMOVE_DUPLICATES USBDevice_INC)
LIST(REMOVE_DUPLICATES USBDevice_SRC)

FOREACH(INC ${USBDevice_INC})
    SET(INC_FILE INC_FILE-NOTFOUND)
    GET_FILENAME_COMPONENT(INC_FILE ${STM32Cube_DIR}/Middlewares/ST/STM32_USB_Device_Library/${INC} DIRECTORY)
    MESSAGE(STATUS "Found ${INC}: ${INC_FILE}")
    LIST(APPEND USBDevice_INCLUDE_DIR ${INC_FILE})
ENDFOREACH()
LIST(REMOVE_DUPLICATES USBDevice_INCLUDE_DIR)

FOREACH(SRC ${USBDevice_SRC})
    SET(SRC_FILE SRC_FILE-NOTFOUND)
    FIND_FILE(SRC_FILE ${SRC}
        HINTS ${STM32Cube_DIR}/Middlewares/ST/STM32_USB_Device_Library
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    MESSAGE(STATUS "Found ${SRC}: ${SRC_FILE}")
    LIST(APPEND USBDevice_SOURCES ${SRC_FILE})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(USBDevice DEFAULT_MSG USBDevice_INCLUDE_DIR USBDevice_SOURCES)
