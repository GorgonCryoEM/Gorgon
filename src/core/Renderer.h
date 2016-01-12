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
        virtual void setSpacing(float spX, float spY, float spZ);
        virtual float getSpacingX() const;
        virtual float getSpacingY() const;
        virtual float getSpacingZ() const;
        virtual void setOrigin(float orgX, float orgY, float orgZ);
        virtual float getOriginX() const;
        virtual float getOriginY() const;
        virtual float getOriginZ() const;

        float getMin(int dimension) const;
        float getMax(int dimension) const;

    protected:
        float minPts[3];
        float maxPts[3];
        float spacing[3];
        float origin[3];
    };

    Renderer::Renderer() {
        setSpacing(1.0f, 1.0f, 1.0f);
        setOrigin(0.0f, 0.0f, 0.0f);
    }

    Renderer::~Renderer() {
    }

    float Renderer::getMin(int dimension) const {
        return minPts[dimension];
    }

    float Renderer::getMax(int dimension) const {
        return maxPts[dimension];
    }

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

    void Renderer::setSpacing(float spX, float spY, float spZ) {
        spacing[0] = spX;
        spacing[1] = spY;
        spacing[2] = spZ;
    }

    float Renderer::getSpacingX() const {
        return spacing[0];
    }

    float Renderer::getSpacingY() const {
        return spacing[1];
    }

    float Renderer::getSpacingZ() const {
        return spacing[2];
    }

    void Renderer::setOrigin(float orgX, float orgY, float orgZ) {
        origin[0] = orgX;
        origin[1] = orgY;
        origin[2] = orgZ;
    }


    float Renderer::getOriginX() const {
        return origin[0];
    }

    float Renderer::getOriginY() const {
        return origin[1];
    }

    float Renderer::getOriginZ() const {
        return origin[2];
    }
}


#endif
