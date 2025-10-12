//
// Created by pedro-souza on 12/10/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include "Platforms.h"

typedef enum {
    SERVER_DOWN = 0,
    SERVER_READY,
    SERVER_RUNNING,
    SERVER_STOPPED,
    SERVER_ERROR,
} NetServerState;

typedef struct {
    NetServerState state;
    int socket;

    pthread_t receive_t;
    pthread_mutex_t server_mutex;
} NetServer;


void init_server(NetServer *server);
void start_server_operations(NetServer *server);
void read_inputs(NetServer *server);
void finish_server_operations(NetServer *server);
void close_server(NetServer *server);

#endif //SERVER_H