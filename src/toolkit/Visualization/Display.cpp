/*
 * Display.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Display.h"
#include "IsoSurface.h"
#include "CrossSection.h"
#include "Solid.h"

#include "macros.h"

using namespace Visualization;

Display::Display()
    : DisplayBase()
{
    displays.push_back(new IsoSurface  );
    displays.push_back(new CrossSection);
    displays.push_back(new Solid       );

    cur = displays[0];
}

#ifdef BASE

void Display::setViewingType(const int type) {
    viewingType = type;
    if(viewingType == VIEWING_TYPE_SOLID) {
        load3DTextureSolidRendering();
    } else if  (viewingType == VIEWING_TYPE_CROSS_SECTION) {
        load3DTextureCrossSection();
    }
    calculateDisplay();
}

bool Display::calculateDisplay() {
    return DisplayBase::calculateDisplay();
}

void Visualization::Display::draw(int subSceneIndex, bool selectEnabled) {
    DisplayBase::draw(subSceneIndex, selectEnabled);
}

void Visualization::Display::setSampleInterval(const int size) {
    DisplayBase::setSampleInterval(size);
}

void Visualization::Display::setSurfaceValue(const float value) {
    DisplayBase::setSurfaceValue(value);
}

void Visualization::Display::setMaxSurfaceValue(const float value) {
    DisplayBase::setMaxSurfaceValue(value);
}

bool Visualization::Display::setCuttingPlane(float position, float vecX,
                                             float vecY, float vecZ)
{
    return DisplayBase::setCuttingPlane(position, vecX, vecY, vecZ);
}
#else
bool Display::calculateDisplay() {
    return cur->calculateDisplay();
}

void Visualization::Display::draw(int subSceneIndex, bool selectEnabled) {
    cur->draw(subSceneIndex, selectEnabled);
}

void Visualization::Display::setSampleInterval(const int size) {
    cur->setSampleInterval(size);
}

void Visualization::Display::setSurfaceValue(const float value) {
    cur->setSurfaceValue(value);
}

void Visualization::Display::setMaxSurfaceValue(const float value) {
    cur->setMaxSurfaceValue(value);
}

bool Visualization::Display::setCuttingPlane(float position, float vecX,
                                             float vecY, float vecZ)
{
    return cur->setCuttingPlane(position, vecX, vecY, vecZ);
}
//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   Display.cpp"<<endl;
          cout<<"DEBUG: Method: Display::load(string)\033[0m"<<endl;
          cout<<"this: "<<this<<endl;
          cout<<getSize()<<endl;
//    #endif

#endif

