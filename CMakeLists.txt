cmake_minimum_required(VERSION 3.18)

project(capjs VERSION 1.2.1 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

option(HAVE_MY_BOOL "mysql has my_bool type" OFF)

if (HAVE_MY_BOOL)
  add_definitions(-DHAVE_TYPE_MY_BOOL)
endif()

option(TRACE_CAPACITY "trace string capacity" OFF)

if (TRACE_CAPACITY)
  add_definitions(-DTRACE_CAPACITY)
endif()

option(STATIC_STDCPP "static linking of libstdc++" ON)

add_subdirectory(btdef)

set(sources
    src/json_array.cpp
    src/json_object.cpp
    src/json_date.cpp
    src/json_time.cpp
    src/json_write.cpp
    src/mkkv.cpp
    src/journal.cpp
)

include_directories("/usr/include/mysql")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(capjs SHARED ${sources})

if (STATIC_STDCPP)
    target_link_libraries(capjs LINK_PRIVATE btdef -static-libgcc -static-libstdc++ -l:libstdc++.a)
else()
  target_link_libraries(capjs LINK_PRIVATE btdef)
endif()



