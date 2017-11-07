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

Object object_create(
        unsigned long long id,
        double ra, double orthoSD,
        double dec, double decSD) {

    double sd = (orthoSD > decSD) ? orthoSD : decSD; /* What is this? */

    Object object;
    object.id       = id;
    object.ra       = ra  * M_PI/180;;
    object.dec      = dec * M_PI/180;
    object.sd       = sd * M_PI/180; /* What is SD? */

	return (object);

}

