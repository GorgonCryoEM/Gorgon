#ifndef TOOLKIT_MESH_RENDERER_H
#define TOOLKIT_MESH_RENDERER_H

//#include <cstdlib>
//#include <cstdio>
//#include <GorgonGL.h>
//#include <string>
#include <Core/volume.h>
#include <Readers/reader.h>
#include <Visualization/NonManifoldMesh.h>
#include "Renderer.h"
#include <Foundation/StringUtils.h>

using namespace Protein_Morph;
//using namespace GraySkeletonCPP;
using namespace Foundation;
using namespace SkeletonMaker;

namespace Visualization {
    class MeshRenderer : public Volume, public Renderer {
    public:
        NonManifoldMesh getMesh();
        Vector3Float get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
        void loadFile(string fileName);
        void loadVolume(Volume * sourceVolume);
        void saveFile(string fileName);
        void PerformSmoothLaplacian(double convergenceRate, int iterations);
        string getSupportedLoadFileFormats();
        string getSupportedSaveFileFormats();
        int IntersectMeshAndSphere(Vector3Float center, float radius);
        Vector3Float getIntersectionPoint(int ix);

        void SetLineThickness(int thickness);
        void Draw(int subSceneIndex, bool selectEnabled);
    private:
        NonManifoldMesh mesh;
        vector<Vector3Float> intersectionPoints;
        int lineThickness;
    };


    void MeshRenderer::Draw(int subSceneIndex, bool selectEnabled) {
        switch(subSceneIndex) {
            case 0:
                    mesh.Draw(false, false, true, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
                break;
            case 1:
                    mesh.Draw(false, true, false, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
                break;
            case 2:
                    mesh.Draw(true, false, false, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
                break;
        }
    }

    void MeshRenderer::SetLineThickness(int thickness) {
        lineThickness = thickness;
    }

    NonManifoldMesh MeshRenderer::getMesh() {
        return mesh;
    }

    Vector3Float MeshRenderer::getIntersectionPoint(int ix) {
        return intersectionPoints[ix];
    }


    void MeshRenderer::loadFile(string fileName) {
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);
        extension = StringUtils::StringToUpper(extension);

#ifdef GORGON_DEBUG
      cout<<"\033[36mDEBUG: File:   MeshRenderer.h"<<endl;
      cout<<"DEBUG: Method: MeshRenderer::loadFile\033[0m"<<endl;
      cout<<"DEBUG: Args: string\033[0m"<<endl;
      cout<<"FileName: "<<fileName<<endl;
#endif

        if(extension == "OFF") {
            mesh = *NonManifoldMesh::LoadOffFile(fileName);
        } else if(extension == "MRC" || extension == "ATOM") {
            Volume * volume = MRCReaderPicker::pick(fileName.c_str())->getVolume();
            #ifdef GORGON_DEBUG
                  cout<<"\033[36mDEBUG: After VolumeFormatConverter::LoadVolume(fileName)"<<endl;
                  cout<<"FileName: "<<fileName<<endl;
                  cout<<volume->getSize()<<"\033[0m"<<endl;
            #endif

            mesh = NonManifoldMesh(volume);
#ifdef GORGON_DEBUG
      cout<<"\033[35m"<<mesh.getSize()<<"\033[0m"<<endl;
#endif
            delete volume;
        } else {
            cout<<"Input format "<<extension<<" not supported!"<<endl;
        }
    }

    void MeshRenderer::saveFile(string fileName) {
            int pos = fileName.rfind(".") + 1;
            string extension = fileName.substr(pos, fileName.length()-pos);

            extension = StringUtils::StringToUpper(extension);

            if(extension == "OFF") {
                mesh.ToOffCells(fileName);
            } else if(extension == "MRC") {
                Volume * volume = mesh.ToVolume();
                volume->toMRCFile(fileName.c_str());
                delete volume;
            } else {
              cout<<"Input format "<<extension<<" not supported!"<<endl;
            }
    }

    void MeshRenderer::loadVolume(Volume * sourceVolume) {
        mesh = new NonManifoldMesh(sourceVolume);
    }

    void MeshRenderer::PerformSmoothLaplacian(double convergenceRate, int iterations) {
        mesh = mesh.SmoothLaplacian(convergenceRate, iterations);
    }

    int MeshRenderer::IntersectMeshAndSphere(Vector3Float center, float radius) {
        float x1, y1, z1, x2, y2, z2, x3, y3, z3, r, a, b, c, d, u;
        Vector3Float p1, p2;
        x3 = center.X();
        y3 = center.Y();
        z3 = center.Z();
        r = radius;
        intersectionPoints.clear();

        for(unsigned int i = 0; i < mesh.edges.size(); i++) {
            p1 = mesh.vertices[mesh.edges[i].vertexIds[0]].position;
            p2 = mesh.vertices[mesh.edges[i].vertexIds[1]].position;
            x1 = p1.X();
            y1 = p1.Y();
            z1 = p1.Z();
            x2 = p2.X();
            y2 = p2.Y();
            z2 = p2.Z();
            a = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
            b = 2*((x2-x1)*(x1-x3) + (y2-y1)*(y1-y3) + (z2-z1)*(z1-z3));
            c = x3*x3 + y3*y3 + z3*z3 + x1*x1 + y1*y1 + z1*z1 - 2*(x3*x1 + y3*y1 + z3*z1) - r*r;
            d = b*b - 4*a*c;

            if(a != 0) {
                if(d >= 0) {
                    u = (-b + sqrt(d))/2*a;
                    if((u >=0) && (u <= 1)) {
                        intersectionPoints.push_back(p1 + (p2 - p1)*u);
                    }
                }
                if (d > 0) {
                    u = (-b - sqrt(d))/2*a;
                    if((u >=0) && (u <= 1)) {
                        intersectionPoints.push_back(p1 + (p2 - p1)*u);
                    }
                }
            }
        }
        return intersectionPoints.size();
    }

    string MeshRenderer::getSupportedLoadFileFormats() {
        return "All Files (*.off *.mrc *.atom);;Volumes (*.mrc *.atom);;Meshes (*.off)";
    }
    string MeshRenderer::getSupportedSaveFileFormats() {
        return "Meshes (*.off);;Volumes (*.mrc)";
    }

    Vector3Float MeshRenderer::get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
        Vector3Float position = Vector3Float(0, 0, 0);
        if((subsceneIndex >= 0) && (ix0 >= 0)) {
            switch(subsceneIndex){
                case 0:
                    for(unsigned int i = 0; i < mesh.faces[ix1].vertexIds.size(); i++) {
                        position += mesh.vertices[mesh.GetVertexIndex(mesh.faces[ix0].vertexIds[i])].position;
                    }
                    position = position * (1.0 / mesh.faces[ix1].vertexIds.size());
                    break;
                case 1:
                    for(unsigned int i = 0; i < 2; i++) {
                        position += mesh.vertices[mesh.GetVertexIndex(mesh.edges[ix0].vertexIds[i])].position;
                    }
                    position = position * 0.5;
                    break;
                case 2:
                    position = mesh.vertices[ix0].position;
                    break;
            }
        }
        return position;
    }


}


#endif
