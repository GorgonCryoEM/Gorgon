/*
 * MeshRenderer.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_MESHRENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_MESHRENDERER_H_

//#include <cstdlib>
//#include <cstdio>
//#include <GorgonGL.h>
//#include <string>
#include <Core/volume.h>
#include <Readers/reader.h>
#include <Visualization/NonManifoldMesh.h>
#include "Display.h"
#include <Foundation/StringUtils.h>

using namespace Core;
//using namespace GraySkeletonCPP;
using namespace Foundation;
using namespace SkeletonMaker;

namespace Visualization {

    class MeshRenderer : public Display {
        public:
            NonManifoldMesh getMesh();
            void load(string fileName);
            void save(string fileName);
            int intersectMeshAndSphere(Vec3F center, float radius);
            Vec3F getIntersectionPoint(int ix);

            void setLineThickness(int thickness);
            void draw(int subSceneIndex, bool selectEnabled);

            void loadVolume(Volume src);
        private:
            NonManifoldMesh mesh;
            vector<Vec3F> intersectionPoints;
            int lineThickness;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_MESHRENDERER_H_ */
