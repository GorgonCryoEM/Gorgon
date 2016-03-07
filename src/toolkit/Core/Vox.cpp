/*
 * Vox.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Vox.h"

namespace Core {

    Vox::Vox()
        : vox(3, vector<vector<double> >(3, vector<double>(3)))
    {}

    Vox::~Vox() {}

} /* namespace Core */
