/*
 * Display.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Display.h"

using namespace Visualization;

Display::Display()
    : DisplayBase()
{
    displays.push_back(new IsoSurface);
    displays.push_back(new CrossSection);
    displays.push_back(new Solid);
}

bool Display::calculateDisplay() {
    return DisplayBase::calculateDisplay();
}

