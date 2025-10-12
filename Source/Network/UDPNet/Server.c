//
// Created by pedro-souza on 12/10/2025.
//

#include "Server.h"
#include "Socket.h"
#include "NetLogger.h"
#include "Defs.h"
#include "Addresses.h"
#include "ServerOperations.h"

static void init_server_threads(NetServer *server);
static void join_server_threads(NetServer *server);
static char *read_line();
static void helper();
static void print_local_addr();

void init_server(NetServer *server){
    server->state = SERVER_DOWN;
    server->socket = create_socket_v4();

    bind_socket_to_any_v4(server->socket);

    pthread_mutex_init(&server->server_mutex, NULL);
    server->state = SERVER_READY;

    LOG_MSG(LOG_INFO, "server initialized and ready to start");
}

void start_server_operations(NetServer *server){
    if(server->state == SERVER_READY){
        server->state = SERVER_RUNNING;

        init_server_threads(server);

        LOG_MSG(LOG_INFO, "server operations started successfully");
    }
}

void read_inputs(NetServer *server){
    if(server->state == SERVER_RUNNING){
        printf("type \"help\" to see available options\n");

        while(1){
            pthread_mutex_lock(&server->server_mutex);
            if(server->state == SERVER_STOPPED){
                pthread_mutex_unlock(&server->server_mutex);
                break;
            }
            pthread_mutex_unlock(&server->server_mutex);

            printf("> ");
            fflush(stdout);

            char *line = read_line();
            if (line == NULL) {
                continue;
            }

            if (strlen(line) == 0) {
                continue;
            }

            if(strcmp(line, "help") == 0){
                helper();
            }

            else if(strcmp(line, "addr") == 0){
                print_local_addr();
            }

            else if(strcmp(line, "quit") == 0){
                printf("closing the server...\n");
                pthread_mutex_lock(&server->server_mutex);
                server->state = SERVER_STOPPED;
                pthread_mutex_unlock(&server->server_mutex);
            }

            else if(strcmp(line, "peers") == 0){

            }
        }
    }
}

void finish_server_operations(NetServer *server){
    if(server->state == SERVER_STOPPED){
        join_server_threads(server);
    }

    LOG_MSG(LOG_INFO, "server finished successfully");
}

void close_server(NetServer *server) {
    if(server->state == SERVER_STOPPED){
        pthread_mutex_destroy(&server->server_mutex);

        if(close_socket(server->socket) < 0){
            sys_log_exit("socket closure failed");
        }

        server->state = SERVER_DOWN;
    }

    LOG_MSG(LOG_INFO, "server closed successfully");
}

static void init_server_threads(NetServer *server){
    if(pthread_create(&server->receiver_t, NULL, receiver_thread, server) != 0){
        sys_log_exit("receiver thread creation failure");
    }
}

static void join_server_threads(NetServer *server) {
    if(pthread_join(server->receiver_t, NULL) != 0){
        sys_log_exit("receiver thread join failure");
    }
}

static char *read_line(){
    static char input[MAX_INPUT_SIZE];

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return NULL;
    }

    input[strcspn(input, "\n")] = '\0';

    return input;
}

static void helper(){
    printf("help: show available commands\n");
    printf("addr : show the connection address\n");
    printf("quit: gracefully exit the program\n");
    printf("peers: show the current peers\n");
}

static void print_local_addr(){
    printf("PORT: %d\n", APP_PORT);
    printf("LOCAL IP: 127.0.0.1\n");

    char *ip = get_lan_ip_v4();

    if(ip != NULL){
        printf("PUBLIC LAN IP: %s\n", ip);
        free(ip);
    }
}