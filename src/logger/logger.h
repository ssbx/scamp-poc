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
    LOGGER_QUIET   = 0,
    LOGGER_NORMAL  = 1,
    LOGGER_VERBOSE = 2,
    LOGGER_DEBUG   = 3
} LoggerLevel;

/*
 * Set log level, can be of the enum LoggerLevel.
 */
extern void Logger_setLevel(int level);

/*
 * Log a message.
 */
extern void Logger_log(LoggerLevel level, char *format, ...);

#endif /* __LOGGER_H__ */
