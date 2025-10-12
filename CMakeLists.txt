cmake_minimum_required(VERSION 3.26)
set(CMAKE_C_STANDARD 17)

set(PROJECT_NAME "game-server")
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} Source/main.c
        Source/Network/UDPNet/Server.c
        Source/Network/UDPNet/Server.h
        Source/Network/UDPNet.h
        Source/Network/UDPNet/Platforms.h
        Source/Network/UDPNet/Platforms.c
        Source/Network/UDPNet/NetLogger.c
        Source/Network/UDPNet/NetLogger.h
        Source/Network/UDPNet/Socket.c
        Source/Network/UDPNet/Socket.h
        Source/Network/UDPNet/Addresses.c
        Source/Network/UDPNet/Addresses.h
        Source/Network/UDPNet/Defs.h)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif ()