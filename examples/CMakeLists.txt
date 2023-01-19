
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

install (TARGETS unadf adf_floppy_create adf_floppy_format )
