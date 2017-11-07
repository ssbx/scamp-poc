/*
 * Logger utils.
 *
 * Copyright (C) 2017 University of Bordeaux. All right reserved.
 * Written by Emmanuel Bertin
 * Written by Sebastien Serre
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

static LogLevel LOG_LEVEL = LOG_NORMAL;

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
