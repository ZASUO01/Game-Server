//
// Created by pedro-souza on 12/10/2025.
//
#undef  main
#include "Network/Platforms.h"
#include "Network/Server.h"
#include <cstdlib>

int main(){
    if (Platforms::platformNetworkingInit() != 0) {
        exit(EXIT_FAILURE);
    }

    auto *server = new NetServer();
    server->Init();
    server->Run();
    server->Shutdown();

    delete server;
    server = nullptr;

    Platforms::platformNetworkingCleanup();
    return 0;
}
