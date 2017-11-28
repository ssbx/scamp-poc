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
    LOGGER_CRITICAL = 0,
    LOGGER_ERROR    = 3,
	LOGGER_QUIET    = 5,
	LOGGER_NORMAL   = 10,
	LOGGER_VERBOSE  = 15,
	LOGGER_DEBUG    = 20,
	LOGGER_TRACE    = 25
} LoggerLevel;

/*
 * Set log level.
 */
extern void
Logger_setLevel(LoggerLevel level);

/*
 * Log a message.
 * TODO put this in a macro and print file and line number.
 */
extern void
Logger_log(LoggerLevel level, char *format, ...);

#endif /* __LOGGER_H__ */
