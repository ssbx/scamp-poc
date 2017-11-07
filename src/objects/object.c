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
#include "object.h"

/*
 * Create and initialize an Object structure. Inputs are in degree,
 * and transformed in radians.
 */
Object object_create(
        unsigned long long id,
        double ra, double orthoSD,
        double dec, double decSD) {

    double sd = (orthoSD > decSD) ? orthoSD : decSD; /* What is this? */

    Object object;
    object.id  = id;
    object.ra  = ra  * M_PI/180;;
    object.dec = dec * M_PI/180;
    object.sd  = sd  * M_PI/180; /* What is SD? */

	return (object);

}

/* 
 * Determine if the two objects position fit into the "limit"
 * distance.
 *
 */
bool object_areClose(Object a, Object b, double limit) {
    double distance;

    /* See https://fr.wikipedia.org/wiki/Formule_de_haversine */
    distance = 2 * asin(sqrt(
        pow(sin( (b.dec - a.dec) * 0.5) , 2) +
        cos(a.dec) * cos(b.dec) *
        pow(sin((b.ra - a.ra) * 0.5) , 2)
    ));

    if (distance < limit)
        return true;
    else
        return false;

    /* Why??? from github.com/ssbx/crossmatch data/object.cpp
    double distance;
    double term;

    term = sqrt(
        pow(sin( (dec_B - dec_A) * 0.5) , 2) +
        cos(dec_A) * cos(dec_B) *
        pow(sin((ra_B - ra_A) * 0.5) , 2)
    );

    if (term >= 1.0f)
        distance = M_PI;
    else
        distance = 2 * asin(term);
    return distance;
    */

}

