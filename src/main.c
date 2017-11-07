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
#include <unistd.h>
#include "logger/logger.h"
#include "catalog.h"
#include "crossmatch.h"

typedef enum {RUN_TEST, RUN_PROD}        RunType;
typedef enum {FORMAT_ASCII, FORMAT_FITS} FileFormat;
typedef struct {
    RunType    runType;
    FileFormat fileFormat;
    int        loglevel;
    int        numInputFiles;
    double     matchFactor;
    char     **inputFiles;
} OptsInput;

/*
 * Test ascii cross
 */
void test_ascii_simple_cross(char **files, double matchFactor) {
    ObjectList reference, samples;
    reference = catalog_read_ascii_file(files[0]);
    samples   = catalog_read_ascii_file(files[1]);

    crossmatch_run(&reference, &samples, matchFactor);

    objectlist_free(&reference);
    objectlist_free(&samples);
}

/*
 * Our main function
 */
int
main(int argc, char** argv) {
    int opt;
    OptsInput opts_in;


    /* Handle options BEGIN */
    opts_in.loglevel = 1;
    opts_in.runType = RUN_PROD;
    opts_in.fileFormat = FORMAT_ASCII;
    opts_in.inputFiles = NULL;
    opts_in.numInputFiles = 0;

	while ((opt = getopt(argc, argv, "vtcf:")) != -1) {
		switch (opt) {
		case 'v':
			opts_in.loglevel += 1;
			break;
		case 't':
			opts_in.runType = RUN_TEST;
			break;
		case 'c':
			opts_in.fileFormat = FORMAT_FITS;
			break;
		case 'f':
			opts_in.matchFactor = atof(optarg);
			break;
		default:
			abort();

		}
	}

    opts_in.inputFiles = argv + optind;
    opts_in.numInputFiles = argc - optind;
    /* Handle options END */

    logger_set_level(opts_in.loglevel);

    if (opts_in.runType ==  RUN_TEST) {
        if (opts_in.fileFormat == FORMAT_ASCII) {
            test_ascii_simple_cross(opts_in.inputFiles, opts_in.matchFactor);
        } else {
            printf("Start fitscat test\n");
            test_fits_simple_print(opts_in.inputFiles, opts_in.numInputFiles);
        }
    }

    return (EXIT_SUCCESS);

}

