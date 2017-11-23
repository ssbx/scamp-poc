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

#ifndef __CROSSMATCH_H__
#define __CROSSMATCH_H__

#include "objectcat.h"

typedef struct {

} CrossmatchState_T;

CrossmatchState_T Crossmatch_run(
		ObjectCat_T *reference,
		ObjectCat_T *samples,
		double factor);


#endif /* __CROSSMATCH_H__ */
