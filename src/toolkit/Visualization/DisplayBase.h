/*
 * DisplayBase.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_DISPLAYBASE_H_
#define SRC_TOOLKIT_DISPLAYBASE_H_


#define GL_GLEXT_PROTOTYPES
//#define USE_OCTREE_OPTIMIZATION

////#include <iostream>
////#include <GorgonGL.h>
////#include <string>
////#include <GraphMatch/VectorMath.h>
#include "RendererBase.h"
////#include "GlobalConstants.h"
////#include "MeshRenderer.h"
#include <Core/volume.h>
////#include <GraySkeletonCPP/GlobalDefinitions.h>
////#include <GraySkeletonCPP/VolumeSkeletonizer.h>
////#include <GraySkeletonCPP/VolumeFormatConverter.h>
#include "NonManifoldMesh.h"
#include "TriangleMesh.h"
//#include "MathTools/Vector3.h"
//#include "MathTools/Dim3D.h"
#include "Octree.h"
////#include <queue>
//#include <Readers/reader.h>
//#include <algorithm>
//#include "Mesh.h"
//#include "Foundation/StringUtils.h"

using namespace std;

//using namespace wustl_mm::GraySkeletonCPP;
using namespace Protein_Morph;
//using namespace MathTools;
//using namespace GraphMatch;
//using namespace SkeletonMaker;
using namespace Core;

namespace Visualization {

    /*
     *
     */

    const int VIEWING_TYPE_ISO_SURFACE = 0;
    const int VIEWING_TYPE_CROSS_SECTION = 1;
    const int VIEWING_TYPE_SOLID = 2;
    #ifndef _WIN32
        typedef int PFNGLTEXIMAGE3DPROC;
    #endif

    typedef Octree<Range> VolumeRendererOctreeType;
    typedef OctreeNode<Range> VolumeRendererOctreeNodeType;
    typedef TriangleMesh VolumeSurfaceMeshType;

    class DisplayBase : public Volume, public RendererBase {
        public:
            DisplayBase();
            virtual ~DisplayBase();

            float getSurfaceValue() const;
            int getSampleInterval() const;
            string getSupportedLoadFileFormats();
            string getSupportedSaveFileFormats();
            void enableDraw(bool enable);
            void draw(int subSceneIndex, bool selectEnabled);
            void load(string fileName);
            void save(string fileName);
            void setDisplayRadius(const int radius);
            void setDisplayRadiusOrigin(float radiusOriginX,
                                        float radiusOriginY,
                                        float radiusOriginZ);
            void useDisplayRadius(bool useRadius);
            void setViewingType(const int type);
            void setSampleInterval(const int size);
            void setSurfaceValue(const float value);
            void setMaxSurfaceValue(const float value);
            bool setCuttingPlane(float position, float vecX, float vecY, float vecZ);
            void updateBoundingBox();
            void unload();

    protected:
            bool calculateSurface();
            bool calculateCuttingSurface();
            bool calculateSolidRendering();
            virtual bool calculateDisplay();
            void load3DTextureSolidRendering();
            void load3DTextureCrossSection();
            void initializeOctree();
            void initializeOctreeTag(VolumeRendererOctreeNodeType * node);
            void calculateOctreeNode(VolumeRendererOctreeNodeType * node);
            void MarchingCube(Volume * vol, Mesh * mesh,
                              const float iso_level, int iX, int iY, int iZ,
                              int iScale);
            int smallest2ndPower(int value);
    protected:
        bool drawEnabled;
        bool textureLoaded;
        Dim3D<int> textureSize;
        GLuint textureName;
        float surfaceValue;
        float maxSurfaceValue;
        int sampleInterval;
        int displayRadius;
        int viewingType;
        Volume cuttingVolume;
        Vec3F radiusOrigin;
        bool _useDisplayRadius;

        VolumeSurfaceMeshType * surfaceMesh;
        NonManifoldMesh * cuttingMesh;
        VolumeRendererOctreeType * octree;
        #ifdef _WIN32
            PFNGLTEXIMAGE3DPROC glTexImage3D;
        #endif

    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_DISPLAYBASE_H_ */
