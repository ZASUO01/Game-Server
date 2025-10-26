//
// Created by pedro-souza on 17/10/2025.
//

#ifndef SOCKET_H
#define SOCKET_H
#include "Packet.h"
#include "Platforms.h"

namespace  SocketUtils {
    int createUDPSocketV4();
    void bindSocketToAnyV4(int sock);
    bool socketReadyToReceive(int sock, int ms);
    bool sendPacketToV4(int sock, const NetPacket* pk, size_t pkSize, struct sockaddr_in* addr4);
    bool receivePacketFromV4(int sock, NetPacket *pk, struct sockaddr_in* addr4);
};

#endif //SOCKET_H