//
// Created by pedro-souza on 12/10/2025.
//

#ifndef NET_LOGGER_H
#define NET_LOGGER_H

#include "Platforms.h"

typedef enum {
    LOG_DISABLED,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
} LogLevel;

extern LogLevel current_log_level;

// macro to print formatted logs
#define LOG_MSG(level, fmt, ...) \
do { \
if((level) <= current_log_level && (level) > LOG_DISABLED) { \
static const char * level_names[] = {"NONE","ERROR", "WARNING", "INFO", "DEBUG"}; \
if((level) == LOG_ERROR){ \
fprintf(stderr, "(%s : %d) ", \
__FILE__, __LINE__); \
} \
fprintf(stderr, "[%s] " fmt "\n", \
level_names[level], ##__VA_ARGS__); \
} \
} while(0);

// set the global log level variable
void set_log_level(LogLevel level);

// catch the perror value and exit with message
void sys_log_exit(const char *msg);

// exit with message
void log_exit(const char *msg);

#endif //NET_LOGGER_H