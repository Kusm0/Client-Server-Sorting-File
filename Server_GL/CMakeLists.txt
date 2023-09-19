cmake_minimum_required(VERSION 3.23)
project(Server_GL)

set(CMAKE_CXX_STANDARD 11)

set(target_lib)

if (WIN32)

    list(APPEND target_lib ws2_32)
    set(SOURCES server_win.cpp filesort_win.h)
elseif (UNIX)

    find_package(X11 REQUIRED)
    find_library(LIB_X11 X11)
    include_directories(${X11_INCLUDE_DIR})

    list(APPEND target_lib pthread ${LIB_X11})
    set(SOURCES server_linux.cpp filesort_linux.h)
endif ()

add_executable(Server_GL ${SOURCES})
target_link_libraries(Server_GL ${target_lib})

