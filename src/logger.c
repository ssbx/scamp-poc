#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

LogLevel LOG_LEVEL = LOG_NORMAL;

void logger_set_level(int level) {
    if (level > LOG_DEBUG)
        level = LOG_DEBUG;
    LOG_LEVEL = level;
}

void logger_log(LogLevel level, char *format, ...) {
    va_list args;

    if (level <= LOG_LEVEL) {
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}
