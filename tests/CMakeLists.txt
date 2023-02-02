
find_package(CHECK QUIET)
if (NOT CHECK_FOUND)
    find_package(PkgConfig QUIET)
    if (PkgConfig_FOUND)
        set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
        pkg_check_modules ( CHECK check )
    endif (PkgConfig_FOUND)
endif (NOT CHECK_FOUND)

if (NOT CHECK_FOUND)
    # Automatic fetching external dependency
    # ( https://cmake.org/cmake/help/latest/module/FetchContent.html
    #   https://cmake.org/cmake/help/latest/guide/using-dependencies/ )
    message("Fetching Check library")
    include(FetchContent)
    FetchContent_Declare(
      CHECK
      GIT_REPOSITORY https://github.com/libcheck/check.git
      GIT_TAG        0.15.2
    )
    FetchContent_MakeAvailable(CHECK)
endif()

#if (NOT Check_FOUND AND NOT DEFINED Check_QUIET)
#    message(FATAL_ERROR "Could NOT find Check")
#endif (NOT Check_FOUND AND NOT DEFINED Check_QUIET)

#add_compile_options(-pthread)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/src)

#message ("Check include dir: ${CHECK_INCLUDE_DIR}, libraries: ${CHECK_LIBRARIES}")
#if ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
#include_directories(${CHECK_INCLUDE_DIR})
#else()
add_compile_options(${CHECK_CFLAGS})
#endif()

add_executable ( test_adfPos2DataBlock
  test_adfPos2DataBlock.c
)

add_executable ( test_adfDays2Date
  test_adfDays2Date.c
)

if ( "${CHECK_LIBRARIES}" STREQUAL "" )
  set (CHECK_LIBRARIES Check::check)
else()
  #set (CHECK_LIBRARIES "${CHECK_LIBRARIES} -pthread")
  set (CHECK_LIBRARIES "${CHECK_LDFLAGS}")
endif()

target_link_libraries ( test_adfPos2DataBlock PUBLIC
  adf
  ${CHECK_LIBRARIES}
)

target_link_libraries ( test_adfDays2Date PUBLIC
  adf
  ${CHECK_LIBRARIES}
)

configure_file(test_examples test_examples COPYONLY)

add_test ( test_adfPos2DataBlock test_adfPos2DataBlock )
add_test ( test_adfDays2Date test_adfDays2Date )
add_test ( test_examples ${BASH_PROGRAM} test_examples )

