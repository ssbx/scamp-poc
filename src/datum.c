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
