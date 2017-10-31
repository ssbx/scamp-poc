#include "datum.h"

Datum datum_create(
        unsigned long long id,
        double ra, double orthoSD,
        double dec, double decSD) {
    Datum datum;
    datum.id = id;
    datum.ra = ra;
    datum.orthoSD = orthoSD;
    datum.dec = dec;
    datum.decSD = decSD;
    datum.sd = (orthoSD > decSD) ? orthoSD : decSD;

    return datum;
}
