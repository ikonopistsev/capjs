cmake_minimum_required(VERSION 3.0.2)

file(READ "${RELATIVE_SOURCE_DIR}version.txt" FILE_VER)
file(READ "${RELATIVE_SOURCE_DIR}revision.txt" FILE_REV)

# менять верисю пакета тут
project(capjs VERSION ${FILE_VER} LANGUAGES CXX)

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

use_cxx11()

set(sources
    ${RELATIVE_SOURCE_DIR}src/json_array.cpp
    ${RELATIVE_SOURCE_DIR}src/json_object.cpp
    ${RELATIVE_SOURCE_DIR}src/json_date.cpp
    ${RELATIVE_SOURCE_DIR}src/json_time.cpp
    ${RELATIVE_SOURCE_DIR}src/json_write.cpp
    ${RELATIVE_SOURCE_DIR}src/mkkv.cpp
    ${RELATIVE_SOURCE_DIR}src/journal.cpp
)

include_directories("/usr/include/mysql")
include_directories(${RELATIVE_SOURCE_DIR})
include_directories("./")

add_definitions("-DHAVE_DLOPEN")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

add_library(capjs SHARED ${sources})



