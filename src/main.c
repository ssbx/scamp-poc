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
#include "logger.h"
#include "catalog.h"

/*
 * This is where we store our command line arguments
 */
struct global_args {
    int loglevel;
    int numInputFiles;
    char **inputFiles;
} GLOB_ARGS;

/*
 * Our main function
 */
int
main(int argc, char** argv) {
    int opt;

    GLOB_ARGS.loglevel = 1;
    GLOB_ARGS.inputFiles = NULL;
    GLOB_ARGS.numInputFiles = 0;

    while ((opt = getopt(argc,argv, "vq")) != -1) {
        switch (opt)
        {
            case 'v':
                GLOB_ARGS.loglevel += 1;
                break;
            case 'q':
                GLOB_ARGS.loglevel += 0;
                break;
            default:
                abort();

        }
    }

    GLOB_ARGS.inputFiles = argv + optind;
    GLOB_ARGS.numInputFiles = argc - optind;

    logger_set_level(GLOB_ARGS.loglevel);

    catalog_read(GLOB_ARGS.inputFiles, GLOB_ARGS.numInputFiles);

    return EXIT_SUCCESS;
}

