//
// Created by pedro-souza on 12/10/2025.
//
#include "Network/UDPNet.h"
#include "Network/UDPNet/NetLogger.h"
#undef  main

int main(){
    platform_networking_init();

    set_log_level(LOG_DEBUG);
    NetServer server;
    init_server(&server);

    start_server_operations(&server);

    read_inputs(&server);

    finish_server_operations(&server);

    close_server(&server);

    platform_networking_cleanup();
    return 0;
}