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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "logger.h"
#include "catalog.h"
#include "crossmatch.h"

/**
 * TODO:
 * - hash of Object ipring (in field)
 * - array of pointer to objects charing the same ipring
 * - crossmatch
 */
int
main(int argc, char** argv) {

    if (argc < 3)
        Logger_log(LOGGER_CRITICAL, "Require two file arguments\n");

	Logger_setLevel(LOGGER_DEBUG);

	Field field1;
	Field field2;

	Catalog_open(argv[1], &field1);
	Catalog_open(argv[2], &field2);
	// Catalog_dump(&field1);

	Crossmatch_cross(&field1, &field2);

	sleep(60);

	Catalog_free(&field1);
	Catalog_free(&field2);

	return (EXIT_SUCCESS);

}

