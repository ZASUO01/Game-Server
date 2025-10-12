//
// Created by pedro-souza on 12/10/2025.
//
#include "Network/UDPNet.h"

int main(){
    platform_networking_init();

    NetServer server;
    init_server(&server);

    start_server_operations(&server);

    read_inputs(&server);

    finish_server_operations(&server);

    close_server(&server);

    platform_networking_cleanup();
    return 0;
}