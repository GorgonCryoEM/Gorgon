/*
 * CrossSection.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_
#define SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_

#include "Display.h"

namespace Visualization {

    /*
     *
     */
    class CrossSection : public Display {
        public:
            CrossSection();
            virtual ~CrossSection();

            bool calculateDisplay();
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_ */
