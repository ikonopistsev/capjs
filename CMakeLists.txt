cmake_minimum_required(VERSION 3.0.2)

file(READ "version.txt" FILE_VER)

# менять верисю пакета тут
project(capjs VERSION ${FILE_VER} LANGUAGES CXX)

include_directories("/usr/include/mysql")
include_directories("./btdef")

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
    src/json_array.cpp
    src/json_object.cpp
    src/json_date.cpp
    src/json_time.cpp
    src/json_write.cpp
    src/mkkv.cpp
)

add_library(capjs SHARED ${sources})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

install(TARGETS capjs LIBRARY DESTINATION lib/mysql/plugin)
