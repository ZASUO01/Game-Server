//
// Created by pedro-souza on 12/10/2025.
//
#include "NetLogger.h"

LogLevel current_log_level = LOG_INFO;

void set_log_level(LogLevel level){
    current_log_level = level;
}

void sys_log_exit(const char *msg){
    int err = errno;
    fprintf(stderr, "[SYSCALL ERROR] %s: %s\n", msg, strerror(err));
    exit(EXIT_FAILURE);
}

void log_exit(const char *msg){
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
