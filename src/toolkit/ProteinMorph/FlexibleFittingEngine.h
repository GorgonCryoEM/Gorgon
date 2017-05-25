// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for interactively creating and modifying skeletons

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/FlexibleFittingEngine.h,v $
//   $Revision: 1.8 $
//   $Date: 2010/07/29 20:21:59 $
//   $Author: coleman.r $
//   $State: Exp $
//
// History Log: 
//   $Log: FlexibleFittingEngine.h,v $
//   Revision 1.8  2010/07/29 20:21:59  coleman.r
//   gcc compile fix: gcc requires nested templates to end in "> >" not ">>"
//
//   Revision 1.7  2010/07/23 18:18:32  heiderp
//   Side chains now transform correctly.  PDB helices now color correctly and rigid initialization bug is fixed
//
//   Revision 1.6  2010/07/22 21:09:07  heiderp
//   Minor updates. Mostly commenting and removing extra material from CurveDeformer.h
//
//   Revision 1.5  2010/07/19 17:29:02  heiderp
//   LARGE update.  Added flexible fitting functionality, lots of logic in FlexibleFittingEngine.h
//
//   Revision 1.4  2010/06/17 19:31:47  ssa1
//   Visually displaying flexible fitting clusters.
//
//   Revision 1.3  2010/05/21 15:45:16  ssa1
//   Flexible fitting implemented in Gorgon
//
//   Revision 1.2  2010/05/20 21:55:53  ssa1
//   Rigid body alignment based on largest flexible cluster
//
//   Revision 1.1  2010/05/06 21:50:11  ssa1
//   Fixing performance bug when moving a volume
//


#ifndef GORGON_FLEXIBLE_FITTING_ENGINE_H
#define GORGON_FLEXIBLE_FITTING_ENGINE_H

#include <ProteinMorph/SSECorrespondenceFinder.h>
#include <ProteinMorph/SSECorrespondenceFeature.h>
#include <ProteinMorph/SSECorrespondenceNode.h>
#include <ProteinMorph/CurveDeformer.h>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <limits>
#include <MathTools/Vector3.h>
#include <MathTools/LinearSolver.h>
#include <GraphMatch/PDBReader.h>
#include <Readers/SkeletonReader.h>
#include <Visualization/SkeletonMesh.h>
#include <GraphMatch/GeometricShape.h>
#include <Visualization/SSERenderer.h>
#include <eigen3/Eigen/SVD>
#include <eigen3/Eigen/Dense>
#include <ProteinMorph/NonManifoldMesh.h>
#include <MathTools/LinearSolver.h>
//#include <Eigen/SVD>
//#include <Eigen/Dense>

#include <tnt/jama_lu.h>
#include <tnt/jama_qr.h>
#include <tnt/jama_cholesky.h>
#include <boost/tuple/tuple_comparison.hpp>
#include <MathTools/Matrix.h>
#include <eigen3/Eigen/LU>

//#include "SuiteSparse/include/umfpack.h"
//#include "SuiteSparse/include/umfpack_triplet_to_col.h"
//#include "SuiteSparse/include/umfpack_free_symbolic.h"
//#include "SuiteSparse/include/umfpack_free_numeric.h"
//#include "SuiteSparse/UMFPack/include/umfpack.h"
//#include <umfpack.h>
//wustl_mm::
using namespace wustl_mm::Protein_Morph;
using namespace boost::tuples;
using namespace JAMA;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Visualization {	
		class FlexibleFittingEngine {
		public:
			FlexibleFittingEngine();				
			void StartPDBHelix();
			void AddPDBAtomLocation(Vec3F pos);
			void EndPDBHelix();
			void AddSSEHelix(Vec3F pt1, Vec3F pt2);
			vector<Vec3F> Deform(int, MatrixFloat, bool);
			vector<float> returnFlattenedLocations(vector<Vec3F>);
			void addAtomLocation(Vec3F);
			void addHardHandleLocation(Vec3F);
			void addSoftHandleLocation(Vec3F);
			void prepareDeform();
			void setLaplacainW(float);
			void setHardHandleW(float);
			void setSoftHandleW(float);
			void addHelix(int start, int stop);
			void ClearPDBPoints();
			void ResetTransforms();
			void StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold, int maxSolutionCount);
			void AddCorrespondence(int alignmentIx, int SSEIx, int PDBIx);
			void MergeClusters(int alignmentIx, int cluster1, int cluster2);
			int GetCorrespondenceCount();
			int GetClusterCount(int corrIx);
			int GetHelixCount(int corrIx, int clusterIx);
			void ResetEngine();
			SSECorrespondenceNode GetCorrespondenceNode(int corrIx, int clusterIx, int helixIx);
			MatrixFloat GetRigidTransform(int corrIx);
			MatrixFloat GetRigidTransform2(int corrIx, int clusterIx);
			MatrixFloat GetHelixFlexibleTransform(int corrIx, int helixIx);
			vector<bool> GetCorrespondenceFlips(int corrIx);
			vector<int> GetAllCorrespondencesFlat(int corrIx);
			vector<Vec3F> GetAllCAlphaFeatureVecsFlat();
			void FlipCorrespondencePair(int corrIx, int SSEIndex);
			void SaveCorrs();
			void LoadSavedCorrs();
			SSECorrespondenceNode getCorrespondence(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex);
			int getCorrespondenceFeatureCount(const int& alignmentIndex, const int& clusterIndex);
			MatrixFloat GetHelixTranslationTransform(int corrIx, int helixIx);
			MatrixFloat GetPairRigidTransform(int corrIx, int helixIx1, int helixIx2);
			void GetHelixEnds(int corrIx, int helixIx, Vec3F&, Vec3F&);
			bool GetIsForward(int corrIx, int helixIx);
			MatrixFloat GetSideChainTransform(vector<Vec3F> origLocations, vector<Vec3F> newLocations);
			void setDeformationNeighborhoodSizes(const int& newLaplacianNeighborhoodSize, const int& newTransformationNeighborhoodSize, const int& newDeformationNeighborhoodSize);
			void computeCorrespondencePairWiseWeight(const int& selectedAlignmentIndex);
			MatrixFloat getCorrespondenceFeatureTransformation(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex);
			MatrixFloat getCorrespondenceClusterTransformation(const int& alignmentIndex, const int& clusterIndex);
			virtual void loadSequence();
			void addCorrespondenceSourceFeature(const Vec3F&, const Vec3F&);
        	void addCorrespondenceTargetFeature(const Vec3F&, const Vec3F&);
        	void loadHelixFileRenderer(string fileName);
        	void findFitLine(Vec3F & startPosition, Vec3F & endPosition, vector<Vec3F> atomPositions);
        	int getHelixCount();
        	Vec3F getHelixCorner(int helixIx, int cornerIx);
        	void addCorrespondencesByReadIn(const int& pIndex, const int& qIndex, const bool& isForward);
        	void calculateCorrespondences();
        	void addDeformationOriginalVertex(const Vec3F&);
        	void addDeformationEdge(const int& firstVertexIndex, const int& secondVertexIndex);
        	void calculateNeighborVertexIndicesCaches(const int& numVertices);
        	void setDeformationVerticesToOriginalVertices();
        	void addDeformationHandle(const int& vertexIndex, const Vec3F& targetVertex);
        	void deformLaplacian();
        	Array2D<double> transformationForm(const Vec3F&, const int&, const bool&);
	        Array2D<double> transformationForm(const vector<Vec3F>&, const int&, const bool&);
	        
	        // Matrix
	        Array2D<double> inverse(const Array2D<double>&);
	        Array2D<double> transpose(const Array2D<double>&);
	        Array2D<double> linearSolve(const Array2D<double>&, const Array2D<double>&);
	        double* sparseLinearSolve(const int&, const int&, const vector<int>&, const vector<int>&, const vector<double>&, const vector<double>&);
	        vector<Vec3F> GetNonSurfaceVertices();
	        vector<Vec3F> GetSurfaceVertices(int noiseThreshold = 0);
	        void addMeshVertex(float x, float y, float z);
	        void addMeshEdge(int v1, int v2);
	        vector<Vec3F> getDeformedVertices();
	        void setDeformationVerticesToDeformedVertices();
	        void resetDeformationHandles();
	        void calculateDeformationTransformations(const int& numVertices);
	        MatrixFloat getDeformationTransformation(const int& vertexIndex);
	        MatrixFloat findRotationTranslation(vector<Vec3F> sourceCalphaHelixPositions,vector<Vec3F> targetCalphaHelixPositions);
	        void addDeformationRigidInitializationTransformation(const MatrixFloat& rigidInitializationTransformation);
	        void loadOffFile(string filename);
	        void setDeformationWeights(const double& newFittingWeight, const double& newDistortionWeight);
	        void exportSkeletonAsMRC();
	        //void addDeformationHandle(const int&, const Vec3F&);

		private:			
			void BuildSortedHelixDistances(int );

			static const int SAMPLE_COUNT = 10;
			SSECorrespondenceFinder finder;
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			vector<Vec3F> pdbHelixAtomLocs;
			float jointAngleThreshold, dihedralAngleThreshold, lengthThreshold, centroidDistanceThreshold;
			vector<Vec3F> origLocations;
			vector<Vec3F> hardHandles;
			vector<Vec3F> softHandles;
			CurveDeformer cd;
			vector < vector < vector<SSECorrespondenceNode> > > corrs;
			vector<boost::tuples::tuple<int, int, int> > helixDistancesSorted;

			vector < vector < vector<SSECorrespondenceNode> > > savedCorrs;
			vector<SSECorrespondenceFeature> savedFeatureList1;
			vector<SSECorrespondenceFeature> savedFeatureList2;
			Graph sequence;
			int laplacianNeighborhoodSize,
            transformationNeighborhoodSize,
            deformationNeighborhoodSize;

            vector< vector<double> > m_correspondencePairWiseWeight;
			vector<int>  m_correspondenceSourceFeatureIndex;
			vector<int>  m_correspondenceTargetFeatureIndex;
	        vector<bool> m_correspondenceFlipFlag;
			
	        vector<SSECorrespondenceFeature> sourceFeatures,
                                 targetFeatures;
            vector< pair<int, int> > edges;
			double m_bbRatio;
			SSERenderer * sse_renderer;
			int maxCorrespondenceSolutions = 1;

		    int jointAngleDifference;
		    int dihedralAngleDifference;
		    int helixLengthDifference;
		    int helixCentroidDifference;
            vector<Vec3F> originalVertices,
                      vertices,
                      targetVertices,
                      deformedVertices;
            vector< vector<int> > laplacianNeighborVertexIndicesCache,
                      transformationNeighborVertexIndicesCache,
                      deformationNeighborVertexIndicesCache;
            vector<MatrixFloat> rigidInitializationTransformations,deformationTransformations;

            bool rigidInitialization;
                    vector<int> handleIndices;

            float fittingWeight;
    		float distortionWeight;
    		double m_shapeDistance;
    		NonManifoldMesh_NoTags mesh;
    		//LinearSolver::LinearSolver linearSolver;
		};

		inline FlexibleFittingEngine::FlexibleFittingEngine() {
			rigidInitialization = true;
			m_bbRatio = 0.3;
			laplacianNeighborhoodSize = 1;
            transformationNeighborhoodSize = 1;
            deformationNeighborhoodSize = 3;
            maxCorrespondenceSolutions = 1;

		    jointAngleDifference    = 0.0;
		    dihedralAngleDifference = 0.0;
		    helixLengthDifference   = 0.0;
		    helixCentroidDifference = 0.0;

		    fittingWeight    = 1;
    		distortionWeight = 1;

            sse_renderer = new SSERenderer();
            mesh = new NonManifoldMesh_NoTags();
            //finder = new SSECorrespondenceFinder();
			ResetEngine();
		}


		inline void FlexibleFittingEngine::loadSequence() {
	        string type = SEQUENCE_FILE_TYPE; //easier than doing comparison with a char array
	        #ifdef VERBOSE
	            printf("Pattern sequence \n");
	        #endif
	        clock_t start = clock();
	        if (type == "PDB")
	            sequence = *PDBReader::ReadFile(SEQUENCE_FILE_NAME.c_str());
	        clock_t finish = clock();
	        #ifdef VERBOSE
	            printf("\tReading Pattern file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
	            sequence.print();
	        #endif
    	}


		

	}

}

#endif
