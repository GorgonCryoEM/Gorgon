/*
 * Operations.cpp
 *
 *  Created on: Feb 5, 2016
 *      Author: shadow_walker
 */

#include "Operations.h"

using namespace Core;

Operation::Operation(VolumeData &vol)
        : volume(vol)
{}

void Operation::fill(double val)
{
    for(VolumeData::iterator it=volume.begin(); it!=volume.end(); ++it)
                *it = val;
}
