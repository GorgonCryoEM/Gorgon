/*
 * RendererBase.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_
#define SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_

#include "Core/volume.h"
#include "Visualization/MeshBase.h"

using namespace Core;

namespace Visualization {

    class RendererBase {
        public:
            RendererBase();
            virtual void setDisplayStyle(int style);

            virtual float getMinPos(int i) const;
            virtual float getMaxPos(int i) const;

//            virtual int getSizeX() const;
//            virtual int getSizeY() const;
//            virtual int getSizeZ() const;
            virtual float getSpacingX() const;
            virtual float getSpacingY() const;
            virtual float getSpacingZ() const;
            virtual float getOriginX() const;
            virtual float getOriginY() const;
            virtual float getOriginZ() const;

        protected:
            int displayStyle;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
