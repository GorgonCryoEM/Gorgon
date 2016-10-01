/*
 * OpenGLUtils.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_OPENGLUTILS_H_
#define SRC_TOOLKIT_FOUNDATION_OPENGLUTILS_H_

#include <GorgonGL.h>

namespace Core {

    class OpenGLUtils {
        public:
            void static SetColor(float colorR, float colorG, float colorB, float colorA);
            void static GetColor(float &colorR, float &colorG, float &colorB, float &colorA);
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_FOUNDATION_OPENGLUTILS_H_ */
