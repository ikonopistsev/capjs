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

option(HAVE_MY_BOOL "mysql has my_bool type" OFF)
if (HAVE_MY_BOOL)
  add_definitions(-DHAVE_TYPE_MY_BOOL)
endif()

option(TRACE_CAPACITY "trace string capacity" OFF)
if (TRACE_CAPACITY)
  add_definitions(-DTRACE_CAPACITY)
endif()

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

add_library(capjs SHARED ${sources})

option(STATIC_STDCPP "static linking of libstdc++" ON)
if (STATIC_STDCPP)
    target_link_libraries(capjs LINK_PRIVATE -static-libgcc -static-libstdc++ -l:libstdc++.a)
endif()



