/*
 * SkeletonRenderer.cpp
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SkeletonRenderer.h"

namespace Visualization {

    void SkeletonRenderer::draw(int subSceneIndex, bool selectEnabled) {
        switch(subSceneIndex) {
            case 0:
                    SkeletonMesh::draw(false, false, true, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
                break;
            case 1:
                    SkeletonMesh::draw(false, true, false, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
                break;
            case 2:
                    SkeletonMesh::draw(true, false, false, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
                break;
        }
    }

    void SkeletonRenderer::setLineThickness(int thickness) {
        lineThickness = thickness;
    }

    SkeletonMesh SkeletonRenderer::getMesh() {
        return *this;
    }

    Vec3F SkeletonRenderer::getIntersectionPoint(int ix) {
        return intersectionPoints[ix];
    }

    void SkeletonRenderer::load2(string fileName) {
        (SkeletonMesh&)(*this) = SkeletonMesh::loadOffFile2(fileName);
    }

    void SkeletonRenderer::load(string fileName) {
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);
        extension = StringUtils::StringToUpper(extension);

#ifdef GORGON_DEBUG
      cout<<"\033[36mDEBUG: File:   SkeletonRenderer.h"<<endl;
      cout<<"DEBUG: Method: SkeletonRenderer::loadFile\033[0m"<<endl;
      cout<<"DEBUG: Args: string\033[0m"<<endl;
      cout<<"FileName: "<<fileName<<endl;
#endif

        if(extension == "OFF") {
            (SkeletonMesh&)(*this) = SkeletonMesh::loadOffFile(fileName);
        } else if(extension == "MRC" || extension == "ATOM") {
            SkeletonMesh::load(fileName);
            loadVolume((Volume&)(*this));
            #ifdef GORGON_DEBUG
                  cout<<"\033[36mDEBUG: After SkeletonMesh::load(fileName)"<<endl;
                  cout<<"FileName: "<<fileName<<endl;
                  cout<<(Volume)(*this)<<"\033[0m"<<endl;
            #endif

//            mesh = SkeletonMesh(*this);

#ifdef GORGON_DEBUG
      cout<<"\033[35m"<<*this<<"\033[0m"<<endl;
#endif
        } else {
            cout<<"Input format "<<extension<<" not supported!"<<endl;
        }
    }

    void SkeletonRenderer::save(string fileName) {
            int pos = fileName.rfind(".") + 1;
            string extension = fileName.substr(pos, fileName.length()-pos);

            extension = StringUtils::StringToUpper(extension);

            if(extension == "MRC") {
                Volume volume = toVolume();
                volume.toMRCFile(fileName.c_str());
            } else {
              cout<<"Input format "<<extension<<" not supported!"<<endl;
            }
    }

    int SkeletonRenderer::intersectMeshAndSphere(Vec3F center, float radius) {
        float x1, y1, z1, x2, y2, z2, x3, y3, z3, r, a, b, c, d, u;
        Vec3F p1, p2;
        x3 = center.X();
        y3 = center.Y();
        z3 = center.Z();
        r = radius;
        intersectionPoints.clear();

        MUV vertices = getVertices();
        for(unsigned int i = 0; i < curves.size(); i++) {
            p1 = vertices[curves[i].id(0)];
            p2 = vertices[curves[i].id(1)];
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

    void SkeletonRenderer::loadVolume(const Volume & src) {
        static_cast<SkeletonMesh&>(*this) = src;
        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   SkeletonRenderer.cpp"<<endl;
              cout<<"DEBUG: Method: SkeletonRenderer::loadVolume(Volume)\033[0m"<<endl;
              cout<<"src.getSize(): "<<src.getSize()<<endl;
              cout<<"*this:\n"<<*this<<endl;
        #endif

    }

} /* namespace Visualization */
