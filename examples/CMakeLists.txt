
include_directories (
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/generic
  ${PROJECT_BINARY_DIR}/src
)

add_executable ( unadf unadf.c )
TARGET_LINK_LIBRARIES ( unadf adf )

install (TARGETS unadf)
