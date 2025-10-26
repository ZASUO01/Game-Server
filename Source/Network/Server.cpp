//
// Created by pedro-souza on 25/10/2025.
//

#include "Server.h"

#include "NetLogger.h"
#include "Platforms.h"
#include "Socket.h"
#include <iostream>

#include "Addresses.h"
#include "Defs.h"

NetServer::NetServer():
state(NetServerState::SERVER_DOWN)
,socket(-1)
,running(false)
{}

NetServer::~NetServer() {
    Shutdown();
}

void NetServer::Init() {
    if (GetState() != NetServerState::SERVER_DOWN) {
        return;
    }
    SetState(NetServerState::SERVER_READY);

    std::lock_guard<std::mutex> lock(serverMutex);
    socket = SocketUtils::createUDPSocketV4();
    SocketUtils::bindSocketToAnyV4(socket);

    LOG_MSG(LOG_INFO, "Server initialized");
}

void NetServer::Run() {
    if (GetState() != NetServerState::SERVER_READY) {
        return;
    }

    SetRunning(true);
    StartOperations();
    SetState(NetServerState::SERVER_RUNNING);

    ReadInputs();
}

void NetServer::Shutdown() {
    if (GetState() != NetServerState::SERVER_STOPPED) {
        return;
    }
    StopOperations();
    SetState(NetServerState::SERVER_DOWN);

    std::lock_guard<std::mutex> lock(serverMutex);
    if (socket != -1) {
        close_socket(socket);
        socket = -1;
    }
    state = NetServerState::SERVER_DOWN;
    LOG_MSG(LOG_INFO, "Server completely shutdown");
}

void NetServer::StartOperations() {
    receiverThread = std::thread(&NetServer::Receive, this);
    LOG_MSG(LOG_INFO, "Server operations started");
}

void NetServer::StopOperations() {
    if (receiverThread.joinable()) {
        receiverThread.join();
    }

    LOG_MSG(LOG_INFO, "Server operations stopped");
}


void NetServer::Receive() {
    LOG_MSG(LOG_INFO, "Server receiving data started");

    while (true) {
        if (!GetRunning()) {
            break;
        }

        if (!SocketUtils::socketReadyToReceive(socket, 0)) {
            continue;
        }

        struct sockaddr_in clientAddr{};
        NetPacket packet;
        if (!SocketUtils::receivePacketFromV4(socket, &packet, &clientAddr)) {
            continue;
        }

        if (!packet.IsValid()) {
            continue;
        }

        switch (packet.GetFlag()) {
            case NetPacket::DATA_FLAG: {
                auto fields = packet.GetFields();
                std::cout << fields[0].GetValue<std::string>() << "\n";

                NetPacket res(0, NetPacket::DATA_FLAG, 0);
                DataField fd("Hello client");
                res.AddField(fd);
                res.BuildPacket();
                const size_t resSize = NetPacket::PACKET_HEADER_BYTES + res.GetLength();

                SocketUtils::sendPacketToV4(
                    socket,
                    &res,
                    resSize,
                    &clientAddr
                );
                break;
            }
            default:
                break;
        }
    }

    LOG_MSG(LOG_INFO, "Server receiving data finished");
}

void NetServer::ReadInputs() {
    std::cout << "Type \"help\" to see available options\n";

    while (true) {
        std::cout << "> " << std::flush;

        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        const size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) {
            continue;
        }
        const size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);

        if (line.empty()) {
            continue;
        }

        if (line == "help") {
            Helper();
        }
        else if (line == "addr") {
            PrintAddr();
        }
        else if (line == "quit") {
            std::cout << "Closing the server...\n";
            SetRunning(false);
            SetState(NetServerState::SERVER_STOPPED);
            break;
        }
        else if (line == "peers") {

        }
        else {
            std::cout << "Unknown command: " << line << "\n";
            std::cout << "Type 'help' for available options.\n";
        }
    }
}

void NetServer::Helper() {
    std::cout << "help: show available commands\n";
    std::cout << "addr : show the connection address\n";
    std::cout << "quit: gracefully exit the program\n";
    std::cout << "peers: show the current peers\n";
}

void NetServer::PrintAddr() {
    std::cout << "PORT: " << APP_PORT << "\n";
    std::cout << "LOCAL IP: 127.0.0.1\n";

    if (addrString.empty()) {
        addrString = Addresses::getLanIpV4();
    }
    std::cout << "PUBLIC LAN IP: " << addrString << "\n";
}


bool NetServer::GetRunning() {
    std::lock_guard<std::mutex> lock(serverMutex);
    return running;
}

void NetServer::SetRunning(const bool value) {
    std::lock_guard<std::mutex> lock(serverMutex);
    running = value;
}

NetServerState NetServer::GetState() {
    std::lock_guard<std::mutex> lock(serverMutex);
    return state;
}

void NetServer::SetState(const NetServerState value) {
    std::lock_guard<std::mutex> lock(serverMutex);
    state = value;
}

