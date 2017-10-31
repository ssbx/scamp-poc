#ifndef __DATUM_H__
#define __DATUM_H__

typedef struct {
    unsigned long long id;
    double ra;
    double dec;
    double orthoSD;
    double decSD;
    double sd;
} Datum;

Datum datum_create(
        unsigned long long id,
        double ra,
        double dec,
        double orthoSD,
        double decSD);

#endif // __DATUM_H__
