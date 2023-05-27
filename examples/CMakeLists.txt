
include_directories (
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/generic
  ${PROJECT_BINARY_DIR}/src
)

add_executable ( unadf unadf.c )
TARGET_LINK_LIBRARIES ( unadf adf )

add_executable ( adf_floppy_create adf_floppy_create.c )
TARGET_LINK_LIBRARIES ( adf_floppy_create adf )

add_executable ( adf_floppy_format adf_floppy_format.c )
TARGET_LINK_LIBRARIES ( adf_floppy_format adf )

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

TARGET_LINK_LIBRARIES ( adf_show_metadata adf )

file (
  COPY ${PROJECT_SOURCE_DIR}/examples/tests
  DESTINATION ${PROJECT_BINARY_DIR}/examples/
)
add_test ( tests/adf-floppy-test.sh ${BASH_PROGRAM} tests/adf-floppy-test.sh )
add_test ( tests/adf-show-metadata-test.sh ${BASH_PROGRAM} tests/adf-show-metadata-test.sh )
add_test ( tests/unadf-test.sh ${BASH_PROGRAM} tests/unadf-test.sh )

install ( TARGETS
  unadf
  adf_floppy_create
  adf_floppy_format
  adf_show_metadata
)
