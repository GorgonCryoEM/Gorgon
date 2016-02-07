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
    for(int x = 0; x < volume.getSizeX(); x++) {
        for(int y = 0; y < volume.getSizeY(); y++) {
            for(int z = 0; z < volume.getSizeZ(); z++) {
                volume(x, y, z) = val;
            }
        }
    }
}
