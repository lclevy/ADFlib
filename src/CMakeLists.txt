
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

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
  #message("debug mode")
  set ( ADFLIB_DEBUG_MODULES debug_util.c debug_util.h )
endif (CMAKE_BUILD_TYPE MATCHES DEBUG)

add_library ( adf
  adflib.h
  adf_bitm.c
  adf_bitm.h
  adf_blk.h
  adf_cache.c
  adf_cache.h
  adf_dev.c
  adf_dev_dump.c
  adf_dev_dump.h
  adf_dev_flop.c
  adf_dev_flop.h
  adf_dev.h
  adf_dev_hd.c
  adf_dev_hd.h
  adf_dir.c
  adf_dir.h
  adf_env.c
  adf_env.h
  adf_err.h
  adf_file_block.c
  adf_file_block.h
  adf_file.c
  adf_file.h
  adf_file_util.h
  adf_link.c
  adf_link.h
  adf_raw.c
  adf_raw.h
  adf_salv.c
  adf_salv.h
  adf_str.c
  adf_str.h
  adf_types.h
  adf_util.c
  adf_util.h
  adf_version.h
  adf_vol.c
  adf_vol.h
#  debug_util.c
#  debug_util.h
  defendian.h
  hd_blk.h
  prefix.h

  adf_nativ.h
  generic/adf_nativ.c
#  linux/adf_nativ.c
#  win32/adf_nativ.c
#  win32/nt4_dev.c
#  win32/nt4_dev.h

  ${ADFLIB_DEBUG_MODULES}
)

#install(TARGETS adf DESTINATION /usr/local/lib)
install(TARGETS adf)
