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
#include <stdlib.h>

#include "logger.h"

static LoggerLevel L_LEVEL = LOGGER_NORMAL;

void
Logger_setLevel(LoggerLevel level) {
	L_LEVEL = level;
}

void
Logger_log(LoggerLevel level, char *format, ...) {
    FILE *output;
	va_list args;

	if (level < LOGGER_QUIET)
	    output = stderr;
	else
	    output = stdout;

	if (level <= L_LEVEL) {
		va_start(args, format);
		vfprintf(output, format, args);
		va_end(args);
	}

	if (level == LOGGER_CRITICAL)
	    abort();

}
