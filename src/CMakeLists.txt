
configure_file (config.h.cmake.in config.h)

# set DLL symbols export (if building a DLL)
if (ADFLIB_BUILD_DLL)
    add_compile_definitions(_EXPORTING)
endif (ADFLIB_BUILD_DLL)

include_directories (
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/generic
#  ${PROJECT_SOURCE_DIR}/src/linux
#  ${PROJECT_SOURCE_DIR}/src/win32
  ${PROJECT_BINARY_DIR}/src
)

add_library ( adf
  adf_bitm.c
  adf_cache.c
  adf_dev.c
  adf_dev_flop.c
  adf_dev_hd.c
  adf_dir.c
  adf_dump.c
  adf_env.c
  adf_file.c
  adf_file_block.c
  adf_link.c
  adf_raw.c
  adf_salv.c
  adf_util.c
  adf_vol.c
  generic/adf_nativ.c
  generic/adf_nativ.h
#  linux/adf_nativ.c
#  linux/adf_nativ.h
#  win32/adf_nativ.c
#  win32/adf_nativ.h
#  win32/nt4_dev.c
#  win32/nt4_dev.h
)

#install(TARGETS adf DESTINATION /usr/local/lib)
install(TARGETS adf)
