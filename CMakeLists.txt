cmake_minimum_required(VERSION 3.18)

project(capjs VERSION 1.3.0 LANGUAGES CXX)
set (PROJECT_VERSION_RELEASE 1)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

# unsing clang libc++ if needed
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    option(CAPJS_CLANG_LIBCXX "use clang libcxx for ${PROJECT_NAME}" OFF)
    if (CAPJS_CLANG_LIBCXX)
        add_compile_options("-stdlib=libc++")
    endif()
else()
    option(CAPJS_STATIC_STDCPP "static linking of libstdc++" OFF)
endif()

# compiller name for library initilize message
string(CONCAT CAPJS_CXX_NAME "${CMAKE_CXX_COMPILER_ID}" "-" "${CMAKE_CXX_COMPILER_VERSION}")
add_definitions(-DCAPJS_CXX_NAME=${CAPJS_CXX_NAME})

option(CAPJS_HAVE_MY_BOOL "mysql has my_bool type" OFF)
if (CAPJS_HAVE_MY_BOOL)
  add_definitions(-DHAVE_TYPE_MY_BOOL)
endif()

option(CAPJS_MY_COLUMN "use column name as property name" OFF)
if (CAPJS_MY_COLUMN)
  add_definitions(-DUSE_COLUMN_NAME)
endif()

option(CAPJS_TRACE "trace string capacity" OFF)
if (CAPJS_TRACE)
  add_definitions(-DTRACE_CAPACITY)
endif()

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

# include mysql headers
set(MySQL_INCLUDE_DIRS "/usr/include/mysql")
if (NOT EXISTS ${MySQL_INCLUDE_DIRS})
    find_package(PkgConfig)
    pkg_check_modules(MySQL REQUIRED mysqlclient>=5.7)
endif()

include_directories(${MySQL_INCLUDE_DIRS})

add_library(capjs SHARED ${sources})

if (CAPJS_STATIC_STDCPP)
  target_link_libraries(capjs LINK_PRIVATE btdef -static-libgcc -static-libstdc++ -l:libstdc++.a)
else()
  target_link_libraries(capjs LINK_PRIVATE btdef)
endif()

# cpack only for x86_64
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

set(CPACK_PACKAGE_VENDOR "ikonopistsev")
set(CAPJS_HOME_PAGE "https://github.com/ikonopistsev/capjs")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "mysql udf plugin for writing json data")
set(CPACK_PACKAGE_CONTACT "admin@dev4.fun")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_STRIP_FILES TRUE)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if (EXISTS "/etc/os-release")
    # find os release name and version
    file(STRINGS /etc/os-release OS_RELEASE_ID REGEX ^ID= )
    if (OS_RELEASE_ID)
        string(REPLACE "ID=" "" OS_RELEASE_ID ${OS_RELEASE_ID})
        string(REPLACE  "\"" "" OS_RELEASE_ID ${OS_RELEASE_ID}) #"

        file(STRINGS /etc/os-release OS_RELEASE_VERSION_ID REGEX ^VERSION_ID=)
        if (OS_RELEASE_VERSION_ID)
            string(REPLACE "VERSION_ID=" "" OS_RELEASE_VERSION_ID ${OS_RELEASE_VERSION_ID})
            string(REPLACE  "\"" "" OS_RELEASE_VERSION_ID ${OS_RELEASE_VERSION_ID}) #"
            string(CONCAT CPACK_SYSTEM_NAME ${OS_RELEASE_ID} ${OS_RELEASE_VERSION_ID})
        else()
            string(CONCAT CPACK_SYSTEM_NAME ${OS_RELEASE_ID})
        endif()

        if (CMAKE_SYSTEM_PROCESSOR)
            string(CONCAT CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME} "-" ${CMAKE_SYSTEM_PROCESSOR})
        endif()
    endif()
endif()

if (CPACK_GENERATOR STREQUAL "DEB")
    set(CAPJS_LIBRARY_DESTINATION "/usr/lib/${PROJECT_NAME}")
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CAPJS_LIBRARY_DESTINATION})

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ikonopistsev")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_SECTION main)
    set(CPACK_DEBIAN_PACKAGE_RELEASE ${PROJECT_VERSION_RELEASE})
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CAPJS_HOME_PAGE})
    set (CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/dpkg/postinst;${CMAKE_CURRENT_SOURCE_DIR}/dpkg/prerm")
endif()

if (CPACK_GENERATOR STREQUAL "RPM")
    set(CAPJS_LIBRARY_DESTINATION "/usr/lib64/${PROJECT_NAME}")
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CAPJS_LIBRARY_DESTINATION})
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST /usr /usr/lib64)

    set(CPACK_RPM_PACKAGE_RELEASE ${PROJECT_VERSION_RELEASE})
    set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
    set(CPACK_RPM_PACKAGE_URL ${CAPJS_HOME_PAGE})
    SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/postinst")
    SET(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/postrm")
endif()

if (CPACK_GENERATOR STREQUAL "TGZ")
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ".")
endif()

include(CPack)

endif()

