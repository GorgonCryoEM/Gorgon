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
        virtual void LoadFile(string fileName);
        virtual void SaveFile(string fileName);
        virtual void Unload();
        virtual string GetSupportedLoadFileFormats();
        virtual string GetSupportedSaveFileFormats();
        virtual Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);

    };

    Renderer::Renderer()
            : Volume()
    {}

    Renderer::~Renderer() {}

    void Renderer::LoadFile(string fileName) {
        setSpacing(1.0f, 1.0f, 1.0f);
    }

    void Renderer::SaveFile(string fileName) {
    }

    Vector3DFloat Renderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
        return Vector3DFloat(0,0,0);
    }

    void Renderer::Unload() {
    }

    string Renderer::GetSupportedLoadFileFormats() {
        return "All Files (*.*)";
    }

    string Renderer::GetSupportedSaveFileFormats() {
        return "All Files (*.*)";
    }

}


#endif
