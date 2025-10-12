//
// Created by pedro-souza on 12/10/2025.
//
#include "Platforms.h"

int platform_networking_init() {
#if PLATFORM_WINDOWS
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Winsock init failure: %d\n", result);
        return -1;
    }
    printf("Winsock initialized\n");
    return 0;
#else
    return 0;
#endif
}

void platform_networking_cleanup() {
#if PLATFORM_WINDOWS
    WSACleanup();
    printf("Winsock finished\n");
#endif
}