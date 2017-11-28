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

#include "chealpix.h"

/**
 * TODO:
 * 1 - get nsides depending of the max error from all files.
 * 2 - crossmatch with as little as possible objects,
 */
int
main(int argc, char** argv) {
	int i, j, nfields;
	long nsides = 8192; // set to 5120 give strange results (see doc)
	long *cellindex, ncells;
	double radius_arcsec = 2.0; /* in arcsec */

    if (argc < 3)
        Logger_log(LOGGER_CRITICAL, "Require two file arguments\n");

	Logger_setLevel(LOGGER_DEBUG);

	nfields = 2;
	Field fields[nfields];

	/* load fields */
	for (i=0, j=1; i<nfields; i++, j++)
		Catalog_open(argv[j], &fields[i], nsides);


	/* create an effective hash like cell array ... */
	HealpixCell **cells = Crossmatch_initCells(nsides);
	cellindex = Crossmatch_fillCells(fields, nfields, cells, nsides, &ncells);

	/* ... that will speed up cross matching */
//	Crossmatch_crossfields(fields, nfields, cells);
	Crossmatch_crossCells(cells, cellindex, ncells, radius_arcsec);

	/* cleanup */
	for (i=0; i<nfields; i++)
		Catalog_freeField(&fields[i]);
	 Crossmatch_freeCells(cells, nsides);

	 Logger_log(LOGGER_DEBUG, "For nside 16384, size is %li MB\n", sizeof(void*) * (nside2npix(10240) / 1000000));

	return (EXIT_SUCCESS);

}

