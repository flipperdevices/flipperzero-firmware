SET(uGFX_COMPONENTS gfx gadc gaudio gdisp_mcufont gdisp_fonts
    gdisp_image gdriver gdisp gevent gfile ginput gmisc gos
    gdisp_pixmap gqueue gtimer gtrans gwin
    )

SET(uGFX_GDISP_DRIVERS ED060SC4  AlteraFramereader framebuffer ED060SC4 Fb24bpp
    HX8347D ILI93xx ILI9320 ILI9325 ILI9341 ILI9481 LGDP4532 Nokia6610GE8
    Nokia6610GE12 PCD8544 PCF8812 R61505U RA8875 S6D1121 SPFD54124B SSD1289
    SSD1306 SSD1331 SSD1351 SSD1963 SSD2119 ST7565
    # STM32F429iDiscovery
    STM32LTDC TestStub TLS8204)

SET(uGFX_REQUIRED_COMPONENTS gfx gos gdisp gdriver)
LIST(APPEND uGFX_COMPONENTS ${uGFX_REQUIRED_COMPONENTS})
LIST(APPEND uGFX_COMPONENTS ${uGFX_GDISP_DRIVERS})

SET(uGFX_PREFIX gfx)

SET(uGFX_HEADERS
    ${uGFX_PREFIX}.h
    ${uGFX_PREFIX}_options.h
    ${uGFX_PREFIX}_types.h
    ${uGFX_PREFIX}_compilers.h
    )

SET(uGFX_SRCS
    ${uGFX_PREFIX}.c
    ${uGFX_PREFIX}_mk.c
    )

if(NOT uGFX_LLD_CONFIG)
    MESSAGE("No uGFX_LLD_CONFIG given, this may result in an error")
ELSE()
    LIST(APPEND uGFX_INCLUDE_DIRS ${uGFX_LLD_CONFIG})
ENDIF()

# Set defaults if no components given
IF(NOT uGFX_FIND_COMPONENTS)
    SET(uGFX_FIND_COMPONENTS uGFX_COMPONENTS)
    MESSAGE(STATUS "No uGFX components specified, using all: ${uGFX_COMPONENTS}")
ENDIF()


# Required components
FOREACH(cmp ${uGFX_REQUIRED_COMPONENTS})
    LIST(FIND uGFX_FIND_COMPONENTS ${cmp} uGFX_FOUND_INDEX)
    IF(${uGFX_FOUND_INDEX} LESS 0)
        LIST(APPEND uGFX_FIND_COMPONENTS ${cmp})
    ENDIF()
ENDFOREACH()

IF(NOT uGFX_DRIVERS)
    MESSAGE("No uGFX_DRIVERS set, available drivers: ${uGFX_GDISP_DRIVERS}")
ELSE()
    FOREACH(driver ${uGFX_DRIVERS})
        LIST(FIND uGFX_GDISP_DRIVERS ${driver} DRIVER_INDEX)
        IF(${DRIVER_INDEX} LESS 0)
            LIST(APPEND uGFX_FIND_DRIVERS ${driver})
        ENDIF()
    ENDFOREACH()
ENDIF()

FOREACH(cmp ${uGFX_FIND_COMPONENTS})
    LIST(FIND uGFX_COMPONENTS ${cmp} uGFX_FOUND_INDEX)
    IF(${uGFX_FOUND_INDEX} LESS 0)
        MESSAGE(FATAL_ERROR "Unknown uGFX Module: ${cmp}. Available modules: ${uGFX_COMPONENTS}")
    ELSE()
    ENDIF()
ENDFOREACH()

LIST(REMOVE_DUPLICATES uGFX_FIND_COMPONENTS)

INCLUDE(uGFX_GDISP)

SET(uGFX_gfx_SEARCH_PATH ${uGFX_DIR} ${uGFX_DIR}/src)
SET(uGFX_gfx_HEADERS gfx.h)
SET(uGFX_gfx_SOURCES gfx.c)

SET(uGFX_gadc_SEARCH_PATH ${uGFX_DIR}/src/gadc)
SET(uGFX_gadc_HEADERS gadc_driver.h gadc.h gadc_options.h gadc_rules.h)
SET(uGFX_gadc_SOURCES gadc.c)

SET(uGFX_gaudio_SERCH_PATH ${uGFX_DIR}/src/gaudio)
SET(uGFX_gaudio_HEADERS gadc_driver_record.h gaudio_driver_play.h
    gaudio.h gaudio_options.h gaudio_rules.h)
SET(uGFX_gaudio_SOURCES gaudio.c)

SET(uGFX_gdriver_SEARCH_PATH ${uGFX_DIR}/src/gdriver)
SET(uGFX_gdriver_HEADERS gdriver_options.h gdriver_rules.h gdriver.h)
SET(uGFX_gdriver_SOURCES gdriver.c)

SET(uGFX_gevent_SEARCH_PATH ${uGFX_DIR}/src/gevent)
SET(uGFX_gevent_HEADERS gevent.h gevent_options.h gevent_rules.h)
SET(uGFX_gevent_SOURCES gevent.c)

SET(uGFX_gfile_SEARCH_PATH ${uGFX_DIR}/src/gfile)
SET(uGFX_gfile_HEADERS gfile_fatfs_wrapper.h gfile_fs.h gfile.h gfile_options.h
    gfile_petitfs_wrapper.h gfile_rules.h)
SET(uGFX_gfile_SOURCES gfile.c gfile_fatfs_diskio_chibios.c gfile_fatfs_wrapper.c
    gfile_fs_chibios.c gfile_fs_fatfs.c gfile_fs_mem.c gfile_fs_native.c
    gfile_fs_petitfs.c gfile_fs_ram.c gfile_fs_rom.c gfile_fs_strings.c
    gfile_petitfs_diskio_chibios.c gfile_petitfs_wrapper.c gfile_printg.c
    gfile_scang.c gfile_stdio.c)

SET(uGFX_ginput_SEARCH_PATH ${uGFX_DIR}/src/ginput)
SET(uGFX_ginput_HEADERS ginput_dial.h ginput_driver_dial.h ginput_driver_keyboard.h
    ginput_driver_mouse.h ginput_driver_toggle.h ginput.h ginput_keyboard.h
    ginput_keyboard_microcode.h ginput_mouse.h ginput_options.h ginput_rules.h
    ginput_toggle.h)
SET(uGFX_ginput_SOURCES ginput.c ginput_dial.c ginput_keyboard.c
    ginput_keyboard_microcode.c ginput_mouse.c ginput_toggle.c)

SET(uGFX_gmisc_SEARCH_PATH ${uGFX_DIR}/src/gmisc)
SET(uGFX_gmisc_HEADERS gmisc.h gmisc_options.h gmisc_rules.h)
SET(uGFX_gmisc_SOURCES gmisc_arrayops.c gmisc.c gmisc_hittest.c gmisc_matrix2d.c
    gmisc_trig.c)

SET(uGFX_gos_SEARCH_PATH ${uGFX_DIR}/src/gos)
SET(uGFX_gos_HEADERS
    gos_arduino.h gos_chibios.h gos_cmsis2.h gos_cmsis.h gos_ecos.h
    gos_freertos.h gos.h gos_keil.h gos_linux.h gos_nios.h gos_options.h
    gos_osx.h gos_qt.h gos_raw32.h gos_rawrtos.h gos_rtx5.h gos_rules.h
    gos_win32.h gos_x_heap.h gos_x_threads_cortexm01.h gos_x_threads_cortexm347.h
    gos_x_threads_cortexm47fp.h gos_x_threads.h gos_zephyr.h
    )
SET(uGFX_gos_SOURCES gos_arduino.c gos_chibios.c gos_cmsis2.c gos_cmsis.c
    gos_ecos.c gos_freertos.c gos_linux.c gos_nios.c gos_osx.c gos_raw32.c
    gos_rawrtos.c gos_win32.c gos_x_heap.c gos_x_threads.c gos_zephyr.c)

SET(uGFX_gqueue_SEARCH_PATH ${uGFX_DIR}/src/gqueue)
SET(uGFX_gqueue_HEADERS gqueue.h gqueue_options.h gqueue_rules.h)
SET(uGFX_gqueue_SOURCES gqueue.c)

SET(uGFX_gtimer_SEARCH_PATH ${uGFX_DIR}/src/gtimer)
SET(uGFX_gtimer_HEADERS gtimer.h gtimer_options.h gtimer_rules.h)
SET(uGFX_gtimer_SOURCES gtimer.c)

SET(uGFX_gtrans_SEARCH_PATH ${uGFX_DIR}/src/gtrans)
SET(uGFX_gtrans_HEADERS gtrans.h gtrans_options.h gtrans_rules.h)
SET(uGFX_gtrans_SOURCES gtrans.c)

SET(uGFX_gwin_SEARCH_PATH ${uGFX_DIR}/src/gwin)
SET(uGFX_gwin_HEADERS gwin_button.h gwin_container.h gwin.h gwin_label.h gwin_radio.h
    gwin_textedit.h gwin_checkbox.h gwin_frame.h gwin_image.h gwin_list.h
    gwin_rules.h gwin_widget.h gwin_class.h gwin_gl3d.h gwin_keyboard.h gwin_options.h
    gwin_slider.h gwin_console.h gwin_graph.h gwin_keyboard_layout.h
    gwin_progressbar.h gwin_tabset.h
    )
SET(uGFX_gwin_SOURCES gwin_button.c gwin_container.c gwin_image.c gwin_list.c
    gwin_slider.c gwin_wm.c gwin.c gwin_frame.c gwin_keyboard.c gwin_tabset.c
    gwin_checkbox.c gwin_gl3d.c gwin_keyboard_layout.c gwin_progressbar.c gwin_textedit.c
    gwin_console.c gwin_graph.c gwin_label.c gwin_radio.c gwin_widget.c)

SET(uGFX_gdisp_fonts_SEARCH_PATH
    ${uGFX_DIR}/src/gdisp/fonts
    )

SET(uGFX_gdisp_fonts_HEADERS
    fonts.h
    )
SET(uGFX_gdisp_fonts_SOURCES
    DejaVuSans16_aa.c DejaVuSans24_aa.c DejaVuSansBold12_aa.c fixed_10x20.c
    fixed_7x14.c UI2.c DejaVuSans10.c DejaVuSans16.c DejaVuSans24.c DejaVuSansBold12.c
    DejaVuSans12_aa.c  DejaVuSans20_aa.c  DejaVuSans32_aa.c  fixed_5x8.c
    LargeNumbers.c DejaVuSans12.c DejaVuSans20.c DejaVuSans32.c UI1.c
    )

SET(uGFX_gdisp_mcufont_SEARCH_PATH
    ${uGFX_DIR}/src/gdisp
    ${uGFX_DIR}/src/gdisp/mcufont
    )
SET(uGFX_gdisp_mcufont_HEADERS
    mcufont.h mf_bwfont.h mf_config.h mf_encoding.h mf_font.h mf_justify.h
    mf_kerning.h mf_rlefont.h mf_scaledfont.h mf_wordwrap.h)
SET(uGFX_gdisp_mcufont_SOURCES
    mf_bwfont.c mf_encoding.c mf_font.c mf_justify.c mf_kerning.c mf_rlefont.c
    mf_scaledfont.c mf_wordwrap.c gdisp_fonts.c)

SET(uGFX_gdisp_image_SEARCH_PATH ${uGFX_DIR}/src/gdisp)
SET(uGFX_gdisp_image_HEADERS gdisp_image.h)
SET(uGFX_gdisp_image_SOURCES
    gdisp_image_bmp.c gdisp_image_gif.c gdisp_image_gif.c gdisp_image_jpg.c
    gdisp_image_native.c gdisp_image_png.c gdisp_image.c)

SET(uGFX_gdisp_pixmap_SEARCH_PATH ${uGFX_DIR}/src/gdisp)
SET(uGFX_gdisp_pixmap_HEADERS gdisp_pixmap.h)
SET(uGFX_gdisp_pixmap_SOURCES gdisp_pixmap.c)



FOREACH(comp ${uGFX_FIND_COMPONENTS})
    LIST(FIND uGFX_COMPONENTS ${comp} INDEX)
    IF(INDEX EQUAL -1)
        MESSAGE(FATAL_ERROR "Unknown uGFX component: ${comp}\nSupported uGFX components: ${uGFX_COMPONENTS}")
    ENDIF()
    IF(uGFX_${comp}_SOURCES)
        FOREACH(source ${uGFX_${comp}_SOURCES})
            FIND_FILE(uGFX_${comp}_${source}
                NAMES ${source}
                PATHS ${uGFX_${comp}_SEARCH_PATH}
                NO_DEFAULT_PATH
                CMAKE_FIND_ROOT_PATH_BOTH
                )
            LIST(APPEND uGFX_SOURCES ${uGFX_${comp}_${source}})
        ENDFOREACH()
    ENDIF()
    IF(uGFX_${comp}_HEADERS)
        FOREACH(header ${uGFX_${comp}_HEADERS})
            FIND_PATH(uGFX_${comp}_${header}_INCLUDE_DIR
                NAMES ${header}
                PATHS ${uGFX_${comp}_SEARCH_PATH}
                NO_DEFAULT_PATH
                CMAKE_FIND_ROOT_PATH_BOTH
                )
            LIST(APPEND uGFX_INCLUDE_DIRS ${uGFX_${comp}_${header}_INCLUDE_DIR})
        ENDFOREACH()
    ENDIF()
ENDFOREACH()

LIST(REMOVE_DUPLICATES uGFX_INCLUDE_DIRS)
LIST(REMOVE_DUPLICATES uGFX_SOURCES)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(uGFX DEFAULT_MSG uGFX_INCLUDE_DIRS uGFX_SOURCES)
