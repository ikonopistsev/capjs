cmake_minimum_required(VERSION 3.10)

file(READ "${RELATIVE_SOURCE_DIR}version.txt" FILE_VER)
file(READ "${RELATIVE_SOURCE_DIR}revision.txt" FILE_REV)

# менять верисю пакета тут
project(capjs VERSION ${FILE_VER} LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

option(HAVE_MY_BOOL "mysql has my_bool type" OFF)
if (HAVE_MY_BOOL)
  add_definitions(-DHAVE_TYPE_MY_BOOL)
endif()

option(TRACE_CAPACITY "trace string capacity" OFF)
if (TRACE_CAPACITY)
  add_definitions(-DTRACE_CAPACITY)
endif()

add_subdirectory(btdef)

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

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(capjs SHARED ${sources})

option(STATIC_STDCPP "static linking of libstdc++" ON)
if (STATIC_STDCPP)
    target_link_libraries(capjs LINK_PRIVATE btdef -static-libgcc -static-libstdc++ -l:libstdc++.a)
else()
  target_link_libraries(capjs LINK_PRIVATE btdef)
endif()



