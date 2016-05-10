/*
 * SkeletonRenderer.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_SKELETONRENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_SKELETONRENDERER_H_

//#include <cstdlib>
//#include <cstdio>
//#include <GorgonGL.h>
//#include <string>
#include <Core/volume.h>
#include <Readers/reader.h>
#include "SkeletonMesh.h"
#include "RendererBase.h"
#include <Foundation/StringUtils.h>

using namespace Core;
//using namespace GraySkeletonCPP;
using namespace Foundation;
using namespace SkeletonMaker;

namespace Visualization {

    class SkeletonRenderer : public RendererBase {
        public:
            SkeletonMesh getMesh();
            void load(string fileName);
            void save(string fileName);
            int intersectMeshAndSphere(Vec3F center, float radius);
            Vec3F getIntersectionPoint(int ix);

            void setLineThickness(int thickness);
            void draw(int subSceneIndex, bool selectEnabled);

            void loadVolume(Volume src);
        private:
            SkeletonMesh mesh;
            vector<Vec3F> intersectionPoints;
            int lineThickness;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_SKELETONRENDERER_H_ */
