/*
 * Copyright (C) 2017 University of Bordeaux. All right reserved.
 * Written by Emmanuel Bertin
 * Written by Sebastien Serre
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum {
    LOG_QUIET   = 0,
    LOG_NORMAL  = 1,
    LOG_VERBOSE = 2,
    LOG_DEBUG   = 3
} LogLevel;

void logger_set_level(int level);

void logger_log(LogLevel level, char *format, ...);

#endif /* __LOGGER_H__ */
