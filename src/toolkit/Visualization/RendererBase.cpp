/*
 * RendererBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "RendererBase.h"

using namespace Visualization;

RendererBase::RendererBase() {
}

RendererBase::~RendererBase() {
}

float RendererBase::getMin(int dimension) {
    return minPts[dimension];
}

float RendererBase::getMax(int dimension) {
    return maxPts[dimension];
}

bool RendererBase::selectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
    return false;
}

Vec3F RendererBase::selectionCenterOfMass() {
    return (maxPts - minPts) / 2.0;
}

bool RendererBase::selectionMove(Vec3F moveDirection) {
    //printf("Moving by %f %f %f\n", moveDirection.X(), moveDirection.Y(), moveDirection.Z());
    return false;
}
