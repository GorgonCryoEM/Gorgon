#ifndef SRC_CORE_RENDERER_H
#define SRC_CORE_RENDERER_H

#include <string>
#include <MathTools/Vector3D.h>
//#include <GorgonGL.h>
//#include <Foundation/OpenGLUtils.h>
//#include <MathTools/MathLib.h>

using namespace std;
using namespace MathTools;
//using namespace Foundation;

namespace Visualization {
    class Renderer {
    public:
        Renderer();
        virtual ~Renderer();
        virtual void LoadFile(string fileName);
        virtual void SaveFile(string fileName);
        virtual void Unload();
        virtual string GetSupportedLoadFileFormats();
        virtual string GetSupportedSaveFileFormats();
        virtual Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
        virtual void SetSpacing(float spX, float spY, float spZ);
        virtual float GetSpacingX();
        virtual float GetSpacingY();
        virtual float GetSpacingZ();
        virtual void SetOrigin(float orgX, float orgY, float orgZ);
        virtual float GetOriginX();
        virtual float GetOriginY();
        virtual float GetOriginZ();

        float GetMin(int dimension);
        float GetMax(int dimension);

    protected:
        float minPts[3];
        float maxPts[3];
        float spacing[3];
        float origin[3];
    };

    Renderer::Renderer() {
        SetSpacing(1.0f, 1.0f, 1.0f);
        SetOrigin(0.0f, 0.0f, 0.0f);
    }

    Renderer::~Renderer() {
    }

    float Renderer::GetMin(int dimension) {
        return minPts[dimension];
    }

    float Renderer::GetMax(int dimension) {
        return maxPts[dimension];
    }

    void Renderer::LoadFile(string fileName) {
        SetSpacing(1.0f, 1.0f, 1.0f);
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

    void Renderer::SetSpacing(float spX, float spY, float spZ) {
        spacing[0] = spX;
        spacing[1] = spY;
        spacing[2] = spZ;
    }

    float Renderer::GetSpacingX() {
        return spacing[0];
    }

    float Renderer::GetSpacingY() {
        return spacing[1];
    }

    float Renderer::GetSpacingZ() {
        return spacing[2];
    }

    void Renderer::SetOrigin(float orgX, float orgY, float orgZ) {
        origin[0] = orgX;
        origin[1] = orgY;
        origin[2] = orgZ;
    }


    float Renderer::GetOriginX() {
        return origin[0];
    }

    float Renderer::GetOriginY() {
        return origin[1];
    }

    float Renderer::GetOriginZ() {
        return origin[2];
    }
}


#endif
