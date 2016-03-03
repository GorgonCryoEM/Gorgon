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

using namespace Protein_Morph;
//using namespace GraySkeletonCPP;
using namespace Foundation;
using namespace SkeletonMaker;

namespace Visualization {

    /*
     *
     */
    class MeshRenderer {
        public:
            NonManifoldMesh getMesh();
            Vec3F get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void load(string fileName);
            void save(string fileName);
            void performSmoothLaplacian(double convergenceRate, int iterations);
            int intersectMeshAndSphere(Vec3F center, float radius);
            Vec3F getIntersectionPoint(int ix);

            void setLineThickness(int thickness);
            void draw(int subSceneIndex, bool selectEnabled);
        private:
            NonManifoldMesh mesh;
            vector<Vec3F> intersectionPoints;
            int lineThickness;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_MESHRENDERER_H_ */
