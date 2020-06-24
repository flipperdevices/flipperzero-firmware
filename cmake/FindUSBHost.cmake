IF(NOT STM32Cube_DIR)
    SET(STM32Cube_DIR "/opt/STM32Cube_FW_F1_V1.2.0")
    MESSAGE(STATUS "No STM32Cube_DIR specified, using default: " ${STM32Cube_DIR})
ENDIF()

SET(USBHost_HEADERS 
    Core/Inc/usbh_ctlreq.h
    Core/Inc/usbh_ioreq.h
    Core/Inc/usbh_core.h
    Core/Src/usbh_pipes.h
)

SET(USBHost_SRC
    Core/Src/usbh_ctlreq.c
    Core/Src/usbh_core.c
    Core/Src/usbh_ioreq.c
    Core/Src/usbh_pipes.c
)

SET(USBHost_COMPONENTS CDC AUDIO MSC HID MTP)

ADD_DEFINITIONS(-DUSE_USB_HS)
ADD_DEFINITIONS(-DUSE_HAL_DRIVER)

SET(USBHost_COMPONENTS_CDC_HEADERS
    Class/CDC/Inc/usbh_cdc.h
)
SET(USBHost_COMPONENTS_CDC_SOURCES
    Class/CDC/Src/usbh_cdc.c
)

SET(USBHost_COMPONENTS_AUDIO_HEADERS
    Class/AUDIO/Inc/usbh_audio.h
)
SET(USBHost_COMPONENTS_AUDIO_SOURCES
    Class/AUDIO/Src/usbh_audio.c
)

SET(USBHost_COMPONENTS_MSC_HEADERS
    Class/MSC/Inc/usbh_msc_scsi.h
#    Class/MSC/Inc/usbh_msc_data.h
    Class/MSC/Inc/usbh_msc.h
    Class/MSC/Inc/usbh_msc_bot.h
)
SET(USBHost_COMPONENTS_MSC_SOURCES
    Class/MSC/Src/usbh_msc.c
#    Class/MSC/Src/usbh_msc_data.c
    Class/MSC/Src/usbh_msc_bot.c
    Class/MSC/Src/usbh_msc_scsi.c
)

SET(USBHost_COMPONENTS_HID_HEADERS  
    Class/HID/Inc/usbh_hid_keybd.h
    Class/HID/Inc/usbh_hid_mouse.h
    Class/HID/Inc/usbh_hid_parser.h
    Class/HID/Inc/usbh_hid_usage.h
    Class/HID/Inc/usbh_hid.h
)
SET(USBHost_COMPONENTS_HID_SOURCES
    Class/HID/Src/usbh_hid_keybd.c
    Class/HID/Src/usbh_hid_mouse.c
    Class/HID/Src/usbh_hid_parser.c
    Class/HID/Src/usbh_hid.c
)

SET(USBHost_COMPONENTS_MTP_HEADERS
    Class/MTP/Inc/usbh_mtp_ptp.h
    Class/MTP/Inc/usbh_mtp.h
)
SET(USBHost_COMPONENTS_MTP_SOURCES
    Class/MTP/Src/usbh_mtp_ptp.c
    Class/MTP/Src/usbh_mtp.c
)

IF(NOT USBHost_FIND_COMPONENTS)
    SET(USBHost_FIND_COMPONENTS ${USBHost_COMPONENTS})
    MESSAGE(STATUS "No USBHost components selected, using all: ${USBHost_FIND_COMPONENTS}")
ENDIF()

MESSAGE(STATUS "USBHost components: ${USBHost_FIND_COMPONENTS}")

FOREACH(cmp ${USBHost_FIND_COMPONENTS})
    LIST(FIND USBHost_COMPONENTS ${cmp} USBHost_FOUND_INDEX)
    IF(${USBHost_FOUND_INDEX} LESS 0)
        MESSAGE(FATAL_ERROR "Unknown USBHost component: ${cmp}. Available components: ${USBHost_COMPONENTS}")
    ENDIF()
    LIST(FIND USBHost_COMPONENTS ${cmp} USBHost_FOUND_INDEX)
    IF(NOT (${USBHost_FOUND_INDEX} LESS 0))
        LIST(APPEND USBHost_HEADERS ${USBHost_COMPONENTS_${cmp}_HEADERS})
        LIST(APPEND USBHost_SRC ${USBHost_COMPONENTS_${cmp}_SOURCES})
    ENDIF()
ENDFOREACH()

LIST(REMOVE_DUPLICATES USBHost_HEADERS)
LIST(REMOVE_DUPLICATES USBHost_SRC)

FOREACH(INC ${USBHost_HEADERS})
    SET(INC_FILE INC_FILE-NOTFOUND)
    GET_FILENAME_COMPONENT(INC_FILE ${STM32Cube_DIR}/Middlewares/ST/STM32_USB_Host_Library/${INC} DIRECTORY)
    MESSAGE(STATUS "Found ${INC}: ${INC_FILE}")
    LIST(APPEND USBHost_INCLUDE_DIRS ${INC_FILE})
ENDFOREACH()
LIST(REMOVE_DUPLICATES USBHost_INCLUDE_DIRS)

FOREACH(SRC ${USBHost_SRC})
    SET(SRC_FILE SRC_FILE-NOTFOUND)
    FIND_FILE(SRC_FILE ${SRC}
        HINTS ${STM32Cube_DIR}/Middlewares/ST/STM32_USB_Host_Library
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    MESSAGE(STATUS "Found ${SRC}: ${SRC_FILE}")
    LIST(APPEND USBHost_SOURCES ${SRC_FILE})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(USBHost DEFAULT_MSG USBHost_INCLUDE_DIRS USBHost_SOURCES)
