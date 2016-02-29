/*
 * DisplayBase.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_DISPLAYBASE_H_
#define SRC_TOOLKIT_DISPLAYBASE_H_

#include "RendererBase.h"

namespace Visualization {

    /*
     *
     */
    class DisplayBase : public Core::Volume, public RendererBase {
        public:
            DisplayBase();
            virtual ~DisplayBase();
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_DISPLAYBASE_H_ */
