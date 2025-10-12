//
// Created by pedro-souza on 12/10/2025.
//

#include "Platforms.h"
#include "Socket.h"
#include "NetLogger.h"
#include "Addresses.h"
#include "Defs.h"

int create_socket_v4() {
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        sys_log_exit("socket creation failure");
    }

    return fd;
}

void bind_socket_to_any_v4(int fd){
    struct sockaddr_in addr_in;
    init_any_addr_v4(&addr_in, APP_PORT);

    struct sockaddr *addr = (struct sockaddr *)&addr_in;

    if(bind(fd, addr, sizeof(addr_in)) < 0){
        sys_log_exit("socket binding failure");
    }
}

int socket_ready_to_receive(int fd, int ms){
    POLL_FD_TYPE fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    int result = socket_poll(fds, 1, ms);

    if (result > 0 && (fds[0].revents & POLLIN)) {
        return 0;
    }

    return -1;
}

int send_packet_to_v4(
    int fd,
    UdpNetPacket *pk,
    size_t pk_size,
    struct sockaddr_in *addr
){
    socklen_t addr_size = sizeof(struct sockaddr_in);
    ssize_t bytes_sent = sendto(fd, pk, pk_size, 0, (struct sockaddr *)addr, addr_size);
    if(bytes_sent < 0 || (size_t)bytes_sent != pk_size){
        return -1;
    }

    return 0;
}

int receive_packet_from_v4(int fd, UdpNetPacket *pk, struct sockaddr_in *addr){
    size_t pk_size = PACKET_HEADER_BYTES + MAX_PACKET_DATA_BYTES;
    memset(pk, 0, pk_size);

    socklen_t addr_size = sizeof(struct sockaddr_in);
    ssize_t bytes_received = recvfrom(fd, pk, pk_size, 0, (struct sockaddr *)addr, &addr_size);
    if(bytes_received <= 0){
        return -1;
    }

    return 0;
}