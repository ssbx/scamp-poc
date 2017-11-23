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

#include "logger.h"
#include "catalog.h"
#include "chealpix.h"

/**
 * TODO:
 * - btree for Object ipring
 * - btree for Object id (and set? and field?)
 * - cross match
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

	Catalog_free(&field1);
	Catalog_free(&field2);

	return (EXIT_SUCCESS);

}

