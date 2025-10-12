//
// Created by pedro-souza on 12/10/2025.
//
#include "ServerOperations.h"
#include "NetLogger.h"
#include "Server.h"
#include "Socket.h"
#include  "Packet.h"

void *receiver_thread(void *arg){
    LOG_MSG(LOG_DEBUG, "receiver thread initialized");

    NetServer *server = (NetServer *)arg;

    while(1){
        pthread_mutex_lock(&server->server_mutex);
        if(server->state == SERVER_STOPPED){
            pthread_mutex_unlock(&server->server_mutex);
            break;
        }
        pthread_mutex_unlock(&server->server_mutex);

        if(socket_ready_to_receive(server->socket, 0) != 0){
            continue;
        }

        UdpNetPacket pk;
        struct sockaddr_in client_addr_v4;

        if(receive_packet_from_v4(server->socket, &pk, &client_addr_v4) != 0){
            continue;
        }

        if(is_valid_packet(&pk) != 0){
            continue;
        }

        switch (pk.flag){
            case SYN_FLAG:
                LOG_MSG(LOG_DEBUG, "received SYN_FLAG");
                UdpNetPacket pk2;
                init_packet(&pk2, 0, SYN_ACK_FLAG, 0);
                build_packet(&pk2);
                size_t pk2_size = PACKET_HEADER_BYTES;
                send_packet_to_v4(server->socket, &pk2, pk2_size, &client_addr_v4);
                break;

            default:
                break;
        }
    }

    LOG_MSG(LOG_DEBUG, "receiver thread finished");
    return NULL;
}