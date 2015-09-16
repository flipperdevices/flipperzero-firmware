SET(uGFX_GDISP_MODULES gdisp_fonts gdisp_image gdisp_pixmap)

SET(uGFX_gdisp_SEARCH_PATH ${uGFX_DIR}/src/gdisp)
SET(uGFX_gdisp_HEADERS gdisp_colors.h gdisp_options.h gdisp_rules.h)
SET(uGFX_gdisp_SOURCES gdisp.c)
 
SET(uGFX_gdisp_fonts_SEARCH_PATH ${uGFX_DIR}/src/gdisp ${uGFX_DIR}/src/gdisp/mcufont)
SET(uGFX_gdisp_fonts_HEADERS mcufont.h mf_bwfont.h mf_config.h mf_encoding.h mf_font.h mf_justify.h mf_kerning.h mf_rlefont.h mf_scaledfont.h mf_wordwrap.h)
SET(uGFX_gdisp_fonts_SOURCES mf_bwfont.c mf_encoding.c mf_font.c mf_justify.c mf_kerning.c mf_rlefont.c mf_scaledfont.c mf_wordwrap.c gdisp_fonts.c)

SET(uGFX_gdisp_image_SEARCH_PATH ${uGFX_DIR}/src/gdisp)
SET(uGFX_gdisp_image_HEADERS gdisp_image.h)
SET(uGFX_gdisp_image_SOURCES gdisp_image_bmp.c gdisp_image_gif.c gdisp_image_gif.c gdisp_image_jpg.c gdisp_image_native.c gdisp_image_png.c gdisp_image.c)

SET(uGFX_gdisp_pixmap_SEARCH_PATH ${uGFX_DIR}/src/gdisp)
SET(uGFX_gdisp_pixmap_HEADERS gdisp_pixmap.h)
SET(uGFX_gdisp_pixmap_SOURCES gdisp_pixmap.c)

SET(uGFX_GDISP_DRIVERS ED060SC4 framebuffer HX8347D ILI93xx ILI9320 ILI9325 ILI9341 ILI9481 LGDP4532 
  Nokia6610GE8 Nokia6610GE12 PCD8544 PCF8812 R61505U RA8875 S6D1121 SPFD54124B SSD1289 SSD1306 SSD1331 
  SSD1351 SSD1963 SSD2119 ST7565 STM32F429iDiscovery STM32LTDC TestStub TLS8204)

FOREACH(driver ${uGFX_GDISP_DRIVERS})
  SET(uGFX_driver_${driver}_SEARCH_PATH ${uGFX_DIR}/drivers/gdisp/${driver})
  SET(uGFX_driver_${driver}_SOURCES gdisp_lld_${driver}.c)
  SET(uGFX_driver_${driver}_HEADERS gdisp_lld_config.h)
  LIST(APPEND uGFX_GDISP_MODULES driver_${driver})
ENDFOREACH()
