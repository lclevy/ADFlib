
cmake_minimum_required ( VERSION 2.6 )
project ( adflib )

set( CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake )

set ( adflib_VERSION_MAJOR 0 )
set ( adflib_VERSION_MINOR 7 )
set ( adflib_VERSION_PATCH 12 )

# setting NDEBUG
# ( https://lists.debian.org/debian-mentors/2018/04/msg00244.html )
set ( CMAKE_BUILD_TYPE Release )

enable_testing()

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( regtests/Test )
