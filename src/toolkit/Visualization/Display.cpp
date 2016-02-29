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
    : DisplayBase(), vol(static_cast<Volume &>(*this))
{
    displays.push_back(new IsoSurface);
    displays.push_back(new CrossSection);
    displays.push_back(new Solid);

    cur = displays[0];
}

bool Display::calculateDisplay() {
    return DisplayBase::calculateDisplay();
//    return cur->calculateDisplay();
}

Visualization::Display::~Display() {
    for(vector<DisplayBase *>::iterator it=displays.begin();
            it!=displays.end();
            ++it
            )
    {
        delete *it;
    }
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
