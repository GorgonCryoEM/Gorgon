/*
 * OpenGLUtils.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_OPENGLUTILS_H_
#define SRC_TOOLKIT_VISUALIZATION_OPENGLUTILS_H_


#include "GorgonGL.h"

namespace Foundation {
    class OpenGLUtils {
    public:
        void static SetColor(float colorR, float colorG, float colorB, float colorA);
        void static GetColor(float &colorR, float &colorG, float &colorB, float &colorA);
    };

    void OpenGLUtils::GetColor(float &colorR, float &colorG, float &colorB, float &colorA) {
        GLfloat diffuse[4] = {0,0,0,0};
        glGetMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        colorR = diffuse[0];
        colorG = diffuse[1];
        colorB = diffuse[2];
        colorA = diffuse[3];
    }

    void OpenGLUtils::SetColor(float colorR, float colorG, float colorB, float colorA)  {
        glColor4f(colorR, colorG, colorB, colorA);
        GLfloat diffuse[4] = {colorR, colorG, colorB, colorA};
        GLfloat ambient[4] = {colorR*0.2, colorG*0.2, colorB*0.2, colorA};
        GLfloat specular[4] = {1.0, 1.0, 1.0, 1.0};
        glMaterialfv(GL_BACK, GL_AMBIENT,   ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE,   diffuse);
        glMaterialfv(GL_BACK, GL_SPECULAR,  specular);
        glMaterialf(GL_BACK, GL_SHININESS, 0.1);
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.1);
    }


}


#endif /* SRC_TOOLKIT_VISUALIZATION_OPENGLUTILS_H_ */
