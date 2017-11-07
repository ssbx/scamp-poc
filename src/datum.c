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

#include "math.h"
#include "datum.h"

Datum datum_create(
        unsigned long long id,
        double ra, double orthoSD,
        double dec, double decSD) {
    Datum datum;
    datum.id = id;
    datum.ra = ra;
    datum.raRad = ra * M_PI/180;;
    datum.dec = dec;
    datum.decRad = dec * M_PI/180;
    datum.decSD = decSD;
    datum.orthoSD = orthoSD;
    datum.sd = (orthoSD > decSD) ? orthoSD : decSD;

	return (datum);
}
