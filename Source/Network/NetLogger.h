//
// Created by pedro-souza on 17/10/2025.
//

#ifndef NET_LOGGER_H
#define NET_LOGGER_H

#include <cstdio>

 typedef enum  {
     LOG_DISABLED,
     LOG_ERROR,
     LOG_WARN,
     LOG_INFO,
     LOG_DEBUG
 } LogLevel;

extern LogLevel currentLogLevel;

// macro to print formatted logs
#define LOG_MSG(level, fmt, ...) \
do { \
if((level) <= currentLogLevel && (level) > LOG_DISABLED) { \
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
void setLogLevel(LogLevel level);

namespace NetLogger {
    void sysLogExit(const char *msg);
    void logExit(const char *msg);
};

#endif //NET_LOGGER_H