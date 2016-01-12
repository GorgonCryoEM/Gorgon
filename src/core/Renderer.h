#ifndef SRC_CORE_RENDERER_H
#define SRC_CORE_RENDERER_H

#include <string>
#include <MathTools/Vector3D.h>
//#include <GorgonGL.h>
//#include <Foundation/OpenGLUtils.h>
//#include <MathTools/MathLib.h>
#include <SkeletonMaker/volume.h>

using namespace std;
using namespace MathTools;
//using namespace Foundation;
using namespace SkeletonMaker;

namespace Visualization {
    class Renderer : public Volume {
    public:
        Renderer();
        virtual ~Renderer();
    };

    Renderer::Renderer()
            : Volume()
    {}

    Renderer::~Renderer() {}

}


#endif
