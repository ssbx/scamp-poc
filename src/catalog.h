#ifndef __CATALOG_H__
#define __CATALOG_H__

typedef struct {
    unsigned long long id;
    double ra;
    double dec;
    double orthoSD;
    double decSD;
    double sd;
    
} Datum;

void catalog_read(char **inputFiles, int numInputFiles);

#endif // __CATALOG_H__
