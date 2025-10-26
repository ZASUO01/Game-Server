//
// Created by pedro-souza on 17/10/2025.
//
#include "NetLogger.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include  <cstdlib>

LogLevel currentLogLevel = LOG_INFO;

void setLogLevel(const LogLevel level) {
    currentLogLevel = level;
}

void NetLogger::sysLogExit(const char *msg) {
    const int err = errno;
    fprintf(stderr, "[SYSCALL ERROR] %s: %s\n", msg, strerror(err));
    exit(EXIT_FAILURE);
}

void NetLogger::logExit(const char *msg){
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}