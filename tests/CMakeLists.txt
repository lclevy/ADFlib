
find_package( PkgConfig REQUIRED )

#find_package(Check REQUIRED)
pkg_check_modules ( CHECK REQUIRED check )

#add_compile_options(-pthread)

#configure_file (config.h.cmake.in config.h)
#include_directories(${PROJECT_BINARY_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${CHECK_INCLUDE_DIR})

#link_directories(${CHECK_LIBRARY_DIRS})

add_executable ( test_adfPos2DataBlock
  test_adfPos2DataBlock.c
)


add_test ( test_adfPos2DataBlock test_adfPos2DataBlock )

target_link_libraries ( test_adfPos2DataBlock PUBLIC
  adf
  ${CHECK_LIBRARIES}
  # -lcheck_pic
  -pthread
  #-lrt
  #-lm
  #-lsubunit
)
