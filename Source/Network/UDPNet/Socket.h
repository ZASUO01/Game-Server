//
// Created by pedro-souza on 12/10/2025.
//

#ifndef SOCKET_H
#define SOCKET_H

#include "Packet.h"

int create_socket_v4();

void bind_socket_to_any_v4(int fd);

int socket_ready_to_receive(int fd, int ms);

int send_packet_to_v4(
    int fd,
    UdpNetPacket *pk,
    size_t pk_size,
    struct sockaddr_in *addr
);

int receive_packet_from_v4(int fd, UdpNetPacket *pk, struct sockaddr_in *addr);

#endif //SOCKET_H