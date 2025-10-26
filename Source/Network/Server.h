//
// Created by pedro-souza on 25/10/2025.
//
#ifndef SERVER_H
#define SERVER_H

#include <mutex>
#include <thread>
#include <string>

enum class NetServerState{
    SERVER_DOWN,
    SERVER_READY,
    SERVER_RUNNING,
    SERVER_STOPPED,
    SERVER_ERROR,
};

class NetServer {
public:
    NetServer();
    ~NetServer();
    void Init();
    void Run();
    void Shutdown();

private:
    void StartOperations();
    void StopOperations();
    void Receive();
    void ReadInputs();

    static void Helper();
    void PrintAddr();

    bool GetRunning();
    void SetRunning(bool value);

    NetServerState GetState();
    void SetState(NetServerState value);

    // current server state
    NetServerState state;

    // the server socket
    int socket;

    // server lan ipv4 addr
    std::string addrString;

    // server threads
    bool running;
    std::mutex serverMutex;
    std::thread receiverThread;
};

#endif // SERVER_H