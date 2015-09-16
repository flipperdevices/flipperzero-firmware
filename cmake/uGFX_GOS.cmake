SET(uGFX_GOS_MODULES gos_arduino gos_chibios gos_ecos gos_freertos gos_linux gos_osx gos_raw32 gos_rawrtos gos_win32 gos_x_heap gos_x_threads)

SET(uGFX_gos_SEARCH_PATH ${uGFX_DIR}/src/gos)
SET(uGFX_gos_HEADERS gos.h)

FOREACH(module ${uGFX_GOS_MODULES})
  SET(uGFX_${module}_SEARCH_PATH ${uGFX_DIR}/src/gos)
  SET(uGFX_${module}_SOURCES ${module}.c)
  SET(uGFX_${module}_HEADERS ${module}.h)
ENDFOREACH()

