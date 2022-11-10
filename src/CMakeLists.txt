
configure_file (config.h.cmake.in config.h)

include_directories (
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/generic
  ${PROJECT_BINARY_DIR}/src
)

add_library ( adf
  adf_bitm.c
  adf_cache.c
  adf_dir.c
  adf_disk.c
  adf_dump.c
  adf_env.c
  adf_file.c
  adf_hd.c
  adf_link.c
  adf_raw.c
  adf_salv.c
  adf_util.c
  generic/adf_nativ.c
  generic/adf_nativ.h
)

#install(TARGETS adf DESTINATION /usr/local/lib)
install(TARGETS adf)
