
cmake_minimum_required ( VERSION 3.11 )
project ( adflib C )

set( CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake )

set ( adflib_VERSION_MAJOR 0 )
set ( adflib_VERSION_MINOR 7 )
set ( adflib_VERSION_PATCH 13 )

# setting NDEBUG
# ( https://lists.debian.org/debian-mentors/2018/04/msg00244.html )
#set ( CMAKE_BUILD_TYPE Release )


add_compile_options(
  -Wall
  $<$<CONFIG:RELEASE>:-O3>
)

if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    message("Enable extra compiler options (mainly gcc and clang)")
    add_compile_options(
      -Wextra
      -Wconversion
      -Wsign-conversion
      -pedantic
      -Werror-implicit-function-declaration
    #  -pedantic-errors
    #  $<$<CONFIG:DEBUG>:-g3>
    #  $<$<CONFIG:DEBUG>:-Og>
      $<$<CONFIG:DEBUG>:-ggdb>
    )
endif()

# no address sanitizer for Visual Studio or MinGW
if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" AND NOT ${MINGW})
    message("Enable address sanitizer")

    add_compile_options( $<$<CONFIG:DEBUG>:-fsanitize=address> )
    add_link_options(
    #  $<$<CONFIG:DEBUG>:-lasan>
      $<$<CONFIG:DEBUG>:-fsanitize=address> )
endif()

add_link_options( $<$<CONFIG:DEBUG>:-rdynamic> ) # for backtrace_symbols()


OPTION(ADFLIB_BUILD_DLL "Build Windows DLL" OFF)

if(ADFLIB_BUILD_DLL)
#if(BUILD_SHARED_LIBS)
    add_definitions(-DBUILD_DLL)
endif(ADFLIB_BUILD_DLL)

enable_testing()

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( regtests/Test )

# unit tests (comment out if not needed)
# (requires check framework!)
#if (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    # Visual Studio most likely do not have check - place sources in extern/check
#    ADD_SUBDIRECTORY( extern/check )
#endif()
if (NOT MINGW)
  add_subdirectory( tests )
endif()
