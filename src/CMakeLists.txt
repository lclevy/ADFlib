
# set DLL symbols export (if building a DLL)
if ( ADFLIB_BUILD_DLL )
    message ( STATUS "Exporting DLL symbols." )
    add_compile_definitions ( _EXPORTING )
endif ( ADFLIB_BUILD_DLL )


if ( ADFLIB_ENABLE_NATIVE_DEV )
    if ( WIN32 )
        set ( ADFLIB_NATIVE_DEV_DIR win32 )
        set ( ADFLIB_NATIVE_DEV_SRC
              win32/adf_nativ.c
              win32/nt4_dev.c
              win32/nt4_dev.h )
    else()
        set ( ADFLIB_NATIVE_DEV_DIR linux )
        set ( ADFLIB_NATIVE_DEV_SRC linux/adf_nativ.c )
    endif()
else()
    set ( ADFLIB_NATIVE_DEV_DIR generic )
    set ( ADFLIB_NATIVE_DEV_SRC generic/adf_nativ.c )
endif()
message ( STATUS "Native device directory: ${ADFLIB_NATIVE_DEV_DIR}" )
message ( STATUS "Native device sources:   ${ADFLIB_NATIVE_DEV_SRC}" )

include_directories (
  ${PROJECT_SOURCE_DIR}/src
#  ${PROJECT_SOURCE_DIR}/src/generic
#  ${PROJECT_SOURCE_DIR}/src/linux
#  ${PROJECT_SOURCE_DIR}/src/win32
  ${ADFLIB_NATIVE_DEV_DIR}
  ${PROJECT_BINARY_DIR}/src
)

if ( CMAKE_BUILD_TYPE MATCHES DEBUG )
    message ( STATUS "Debug mode - building with debug_utils")
    set ( ADFLIB_DEBUG_MODULES debug_util.c debug_util.h )
endif ( CMAKE_BUILD_TYPE MATCHES DEBUG )


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
  defendian.h
  hd_blk.h
  prefix.h
  adf_nativ.h
  ${ADFLIB_NATIVE_DEV_SRC}
  ${ADFLIB_DEBUG_MODULES}
)

set_target_properties ( adf PROPERTIES
    #PUBLIC_HEADER "adflib.h"
    PUBLIC_HEADER "*.h"
    #PRIVATE_HEADER ...
    VERSION ${CMAKE_PROJECT_VERSION}
#    SOVERSION ${PROJECT_VERSION_MAJOR}
    SOVERSION 1
)

target_include_directories ( adf
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

#target_link_libraries ( adf ${SOME_LIBRARIES} )

#install(TARGETS adf DESTINATION /usr/local/lib)
install ( TARGETS adf
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/adf"
  PRIVATE_HEADER
#    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/adf"
    DESTINATION FALSE
)
