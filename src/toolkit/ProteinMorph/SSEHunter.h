/*
 * SSEHunter.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_PROTEINMORPH_SSEHUNTER_H_
#define SRC_TOOLKIT_PROTEINMORPH_SSEHUNTER_H_

//#include <Core/volume.h>
#include "Visualization/SkeletonMesh.h"
//#include <vector>
//#include <algorithm>
//#include <map>
//#include <MathTools/Vector3.h>
//#include <MathTools/MathLib.h>
#include <MathTools/CrossCorrelation.h>
#include <GraphMatch/PDBAtom.h>
#include <SkeletonMaker/Skeletonizer.h>
//#include <Gorgon/SkeletonRenderer.h>
//#include <cmath>
#include <iostream>
#include <Core/GlobalDefinitions.h>

//#define USE_TIME_MANAGER
#ifdef USE_TIME_MANAGER
    #include <Foundation/TimeManager.h>
    using wustl_mm::Foundation::TimeManager;
#endif

using namespace std;
using namespace GraphMatch;
//using namespace MathTools;
using namespace GraySkeletonCPP;
//using namespace SkeletonMaker;
using namespace Core;

namespace Protein_Morph {

    enum RadialProfileType {GAUSSIAN, GAUSSIAN_DIP, POLYNOMIAL};

    class SSEHunter {
        public:
            void CreatePseudoAtoms(const Volume & vol, float resolution,
                                   float threshold);
            int GetNumberOfPseudoAtoms();
            PDBAtom& GetPseudoAtom(int i);

            void SetCorrelationScores(const Volume & vol, RadialProfileType type,
                                      float resolution, float deltaAltRadians);
            void SetSkeletonScores(const Volume & vol, const SkeletonMesh & skeleton,
                                   float resolution);
            vector<vector<float> > GetAtomDistances();
            vector<float> GetLocalDirectionalityScores(const Volume & vol);

        private:
            void UpdateMap(Volume & vol, Vec3I loc,
                           Vec3F rangemin, Vec3F rangemax);

            //Ross Coleman: modified from EMAN1 Cylinder.C by Wen Jiang
            float RadialProfile(float r, RadialProfileType type); //r in angstroms
            float RadialProfileGaussian(float r); // r in angstroms
            float RadialProfileGaussianDip(float r); // r in angstroms
            float RadialProfilePolynomial(float r); // r in angstroms

        public:
            void ApplyTemplateCylinder(float cylData[],
                                       int xsize, int ysize, int zsize,
                                       int zFFTPadding, float alt,
                                       float az, RadialProfileType type = POLYNOMIAL,
                                       float len = 16.2, float apix_x = 1,
                                       bool reset = true, float apix_y = -1,
                                       float apix_z = -1);
            void NormThresh(Volume& map, float thresh);
            Volume HelixCorrelation(const Volume & map_vol, RadialProfileType type = POLYNOMIAL,
                                      float length = 16.2,
                                      float deltaAltRadians = 5 * PI / 180,
                                      bool use_mcf = true,
                                      Volume* az_vol = NULL, Volume* alt_vol = NULL);

        private:
            vector<Vec3I> atomVolumePositions; // holds the i, j, k indices that give the voxel position of the pseudoatoms
            vector<PDBAtom> patoms; // pseudoatoms
            static const float max_radius; //for all r > max_radius, RadialProfile(r, {any type}) ~= 0
    };

} /* namespace Protein_Morph */

#endif /* SRC_TOOLKIT_PROTEINMORPH_SSEHUNTER_H_ */
