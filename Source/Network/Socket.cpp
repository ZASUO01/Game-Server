//
// Created by pedro-souza on 17/10/2025.
//

#include "Socket.h"
#include "Platforms.h"
#include "NetLogger.h"
#include <cstring>

#include "Addresses.h"
#include "Defs.h"

int SocketUtils::createUDPSocketV4() {
    const int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        NetLogger::sysLogExit("socket creation failure");
    }

    return fd;
}

void  SocketUtils::bindSocketToAnyV4(const int sock) {
    struct sockaddr_in addr_in{};
    Addresses::initAddrAnyV4(&addr_in, APP_PORT);

    if (const auto* addr = reinterpret_cast<struct sockaddr *>(&addr_in); bind(sock, addr, sizeof(addr_in)) < 0) {
        close_socket(sock);
        NetLogger::sysLogExit("binding failure");
    }
}

bool SocketUtils::socketReadyToReceive(const int sock, const int ms) {
    POLL_FD_TYPE fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    if (const int result = socket_poll(fds, 1, ms); result > 0 && (fds[0].revents & POLLIN)) {
        return true;
    }

    return false;
}


bool SocketUtils::sendPacketToV4(const int sock, const NetPacket* pk, const size_t pkSize, struct sockaddr_in* addr4) {
    constexpr socklen_t addr_size = sizeof(struct sockaddr_in);
    if (const ssize_t bytesSent = sendto(sock, pk, pkSize, 0, reinterpret_cast<struct sockaddr *>(addr4), addr_size);
        bytesSent < 0 || static_cast<size_t>(bytesSent) != pkSize) {
        return false;
        }
    return true;
}

bool SocketUtils::receivePacketFromV4(const int sock, NetPacket *pk, struct sockaddr_in* addr4) {
    constexpr size_t pkSize = NetPacket::PACKET_HEADER_BYTES + NetPacket::MAX_PACKET_DATA_BYTES;
    std::memset(pk, 0, pkSize);

    socklen_t addrSize = sizeof(struct sockaddr_in);
    if (const ssize_t bytesReceived = recvfrom(sock, pk, pkSize, 0, reinterpret_cast<struct sockaddr *>(addr4),
        &addrSize); bytesReceived <= 0) {
        return false;
        }

    return true;
}
