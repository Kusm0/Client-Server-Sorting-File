cmake_minimum_required(VERSION 3.23)
project(Client_GL)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
    # Код для системи Windows
    set(SOURCES client_win.cpp)
elseif(UNIX)
    # Код для систем Unix (включаючи Linux)
    set(SOURCES client_linux.cpp)
endif()

add_executable(Client_GL ${SOURCES})
