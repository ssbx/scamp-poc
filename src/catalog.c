#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "logger.h"
#include "catalog.h"

Datum create_datum(
        unsigned long long id,
        double ra, double orthoSD,
        double dec, double decSD)
{
    Datum datum;
    datum.id = id;
    datum.ra = ra;
    datum.orthoSD = orthoSD;
    datum.dec = dec;
    datum.decSD = decSD;
    datum.sd = (orthoSD > decSD) ? orthoSD : decSD;
    
    return datum;
}

void __read_file(char *fileName) {
    FILE   *file;
    char   *line;
    size_t  len;
    unsigned long long id;
    double ra, orthoSD, dec, decSD;
    
    file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "Error with %s. %s", file, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    while (fscanf(file, "%llu %lf %lf %lf %lf\n", 
                    &id, &ra, &orthoSD, &dec, &decSD) == 5) {
        Datum d = create_datum(id, ra, orthoSD, dec, decSD);
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
