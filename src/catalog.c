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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "datum.h"
#include "logger.h"

static void __read_file(char *fileName) {
    FILE *file;
    unsigned long long id;
    double ra, orthoSD, dec, decSD;
    Datum d;

    if ((file = fopen(fileName, "r")) == NULL) {
        fprintf(stderr, "%s %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%llu %lf %lf %lf %lf\n",
                &id, &ra, &orthoSD, &dec, &decSD) == 5) {
        d = datum_create(id, ra, orthoSD, dec, decSD);
        logger_log(LOG_DEBUG, "%llu %lf %lf %lf %.10f <-\n",
                d.id, d.ra, d.orthoSD, d.dec, d.decSD);
    }
}

void catalog_read(char **inputFiles, int numInputFiles) {
    int i;

#pragma omp parallel for
    for (i=0; i < numInputFiles; i++) {
        __read_file(inputFiles[i]);
    }
}
