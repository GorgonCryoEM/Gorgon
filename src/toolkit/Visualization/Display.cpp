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

using namespace Visualization;

Display::Display()
    : DisplayBase(),
      renderer(static_cast<DisplayBase &>(*this))
{
    displays.push_back(new IsoSurface  (renderer));
    displays.push_back(new CrossSection(renderer));
    displays.push_back(new Solid       (renderer));

    cur = displays[0];
}

bool Display::calculateDisplay() {
    return DisplayBase::calculateDisplay();
//    return cur->calculateDisplay();
}

void Display::setViewingType(const int type) {
    viewingType = type;
    if(viewingType == VIEWING_TYPE_SOLID) {
        load3DTextureSolidRendering();
    } else if  (viewingType == VIEWING_TYPE_CROSS_SECTION) {
        load3DTextureCrossSection();
    }
    calculateDisplay();
}
