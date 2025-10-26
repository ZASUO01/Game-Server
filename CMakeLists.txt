cmake_minimum_required(VERSION 3.26)
set(CMAKE_C_STANDARD 17)

set(PROJECT_NAME "game-server")
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} Source/Main.cpp
        Source/Network/Server.cpp
        Source/Network/Server.h
        Source/Network/Platforms.cpp
        Source/Network/Platforms.h
        Source/Network/Socket.cpp
        Source/Network/Socket.h
        Source/Network/Packet.cpp
        Source/Network/Packet.h
        Source/Network/NetLogger.cpp
        Source/Network/NetLogger.h
        Source/Network/Addresses.cpp
        Source/Network/Addresses.h
        Source/Network/Defs.h
        Source/Network/NetUtils.cpp
        Source/Network/NetUtils.h)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif ()