
include_directories (
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/generic
  ${PROJECT_BINARY_DIR}/src
)


if ( "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" )
  set ( ADFLIB_CUSTOM_GETOPT_SRC
    getopt.c
    getopt.h )
else()
  set ( ADFLIB_CUSTOM_GETOPT_SRC "")
endif()

add_executable ( unadf unadf.c )
target_link_libraries ( unadf adf )
set_target_properties ( unadf PROPERTIES
  #  INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}"
  INSTALL_RPATH "/usr/local/lib"
)

add_executable ( adf_floppy_create adf_floppy_create.c )
target_link_libraries ( adf_floppy_create adf )
set_target_properties ( adf_floppy_create PROPERTIES
  INSTALL_RPATH "/usr/local/lib"
)

add_executable ( adf_format
  adf_format.c
  ${ADFLIB_CUSTOM_GETOPT_SRC}
)
target_link_libraries ( adf_format adf )
set_target_properties ( adf_format PROPERTIES
  INSTALL_RPATH "/usr/local/lib"
)

add_executable ( adf_show_metadata
  adf_show_metadata.c
  adf_show_metadata_common.c
  adf_show_metadata_common.h
  adf_show_metadata_dir.c
  adf_show_metadata_dir.h
  adf_show_metadata_file.c
  adf_show_metadata_file.h
  adf_show_metadata_volume.c
  adf_show_metadata_volume.h
)
target_link_libraries ( adf_show_metadata adf )
set_target_properties ( adf_show_metadata PROPERTIES
  INSTALL_RPATH "/usr/local/lib"
)

add_executable ( adf_bitmap adf_bitmap.c )
target_link_libraries ( adf_bitmap adf )
set_target_properties ( adf_bitmap PROPERTIES
  INSTALL_RPATH "/usr/local/lib"
)

add_executable ( adf_salvage
  adf_salvage.c
  ${ADFLIB_CUSTOM_GETOPT_SRC}
)
target_link_libraries ( adf_salvage adf )
set_target_properties ( adf_salvage PROPERTIES
  INSTALL_RPATH "/usr/local/lib"
)

file (
  COPY ${PROJECT_SOURCE_DIR}/examples/tests
  DESTINATION ${PROJECT_BINARY_DIR}/examples/
)

if ( ADFLIB_ENABLE_TESTS )
    message ( STATUS "Enable tests in examples/" )
    add_test ( tests/test_examples_basic.sh ${BASH_PROGRAM}
               tests/test_examples_basic.sh . )
    add_test ( tests/adf-floppy-test.sh ${BASH_PROGRAM}
               tests/adf-floppy-test.sh )
    add_test ( tests/adf-show-metadata-test.sh ${BASH_PROGRAM}
               tests/adf-show-metadata-test.sh )
    add_test ( tests/unadf-test.sh ${BASH_PROGRAM}
               tests/unadf-test.sh )
endif()

install ( TARGETS
  unadf
  adf_floppy_create
  adf_format
  adf_show_metadata
  adf_bitmap
  adf_salvage
)
