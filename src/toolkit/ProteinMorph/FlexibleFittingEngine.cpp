#include "FlexibleFittingEngine.h"
using namespace Eigen;
namespace wustl_mm {
	namespace Visualization {	
		void FlexibleFittingEngine::ResetEngine() {
					featureList1.clear();
					featureList2.clear();
					hardHandles.clear();
					softHandles.clear();
					origLocations.clear();
					helixDistancesSorted.clear();
		}
		void FlexibleFittingEngine::ClearPDBPoints(){
			featureList1.clear();	
		}
		void FlexibleFittingEngine::ResetTransforms(){
			finder.InitializeFeatures(featureList1, featureList2);
			corrs.clear();
		}


		void FlexibleFittingEngine::StartPDBHelix() {
			pdbHelixAtomLocs.clear();
		}

		void FlexibleFittingEngine::AddPDBAtomLocation(Vec3F pos) {
			pdbHelixAtomLocs.push_back(pos);
		}

		void FlexibleFittingEngine::EndPDBHelix() {
			Vec3F pt1, pt2;
			LinearSolver::FindBestFitLine(pt1, pt2, pdbHelixAtomLocs);
			featureList1.push_back(SSECorrespondenceFeature(pt1, pt2));
		}

		void FlexibleFittingEngine::AddSSEHelix(Vec3F pt1, Vec3F pt2) {
			featureList2.push_back(SSECorrespondenceFeature(pt1, pt2));
		}

		int FlexibleFittingEngine::getHelixCount() {
			return sse_renderer->getHelixCount();
		}

		void FlexibleFittingEngine::addDeformationHandle(const int& vertexIndex, const Vec3F& targetVertex) {
		    handleIndices.push_back(vertexIndex);
		    targetVertices.push_back(targetVertex);
		}


		void FlexibleFittingEngine::findFitLine(Vec3F & startPosition, Vec3F & endPosition, vector<Vec3F> atomPositions) {
			LinearSolver::FindBestFitLine(startPosition, endPosition, atomPositions);
		}

		void FlexibleFittingEngine::calculateDeformationTransformations(const int& numVertices) {
		    deformationTransformations.reserve(numVertices);
		    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
		        vector<Vec3F> sourceNeighborhood,
		                              targetNeighborhood;

		        sourceNeighborhood.push_back(originalVertices[vertexIndex]); 
		        targetNeighborhood.push_back(deformedVertices[vertexIndex]); 

		        vector<int> neighborVertexIndices = deformationNeighborVertexIndicesCache[vertexIndex];
		        for (vector<int>::iterator neighborVertexIndexIterator = neighborVertexIndices.begin(); neighborVertexIndexIterator != neighborVertexIndices.end(); neighborVertexIndexIterator++) {
		            sourceNeighborhood.push_back(originalVertices[*neighborVertexIndexIterator]); 
		            targetNeighborhood.push_back(deformedVertices[*neighborVertexIndexIterator]); 
		        }
		        deformationTransformations.push_back(LinearSolver::FindRotationTranslation(sourceNeighborhood, targetNeighborhood));
		        //deformationTransformations.push_back(linearSolver.FindRotationTranslation(sourceNeighborhood, targetNeighborhood));
		    }

		    //cout << "Done calculating deformation transformations!" << endl;
		}

		MatrixFloat FlexibleFittingEngine::findRotationTranslation(vector<Vec3F> sourceCalphaHelixPositions,vector<Vec3F> targetCalphaHelixPositions) {
			return LinearSolver::FindRotationTranslation(sourceCalphaHelixPositions, targetCalphaHelixPositions);
		}


		void FlexibleFittingEngine::StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold, int maxSolutionCount) {
			finder.InitializeFeatures(featureList1, featureList2);
			finder.InitializeConstants(0, lengthThreshold, 0, 0, 0, 0, 0, jointAngleThreshold, dihedralAngleThreshold, centroidDistanceThreshold, maxSolutionCount, 10);
			finder.PrintFeatureListsMathematica();
			corrs = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(true, true, 4);
		}

		void FlexibleFittingEngine::calculateCorrespondences() {
		    finder.InitializeFeatures(sourceFeatures, targetFeatures); 
		    finder.InitializeConstants(0, helixLengthDifference, 0, 0, 0, 0, 0, jointAngleDifference, dihedralAngleDifference, helixCentroidDifference, maxCorrespondenceSolutions, 10);
		    
		    corrs = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, true, 4);
		}

		void FlexibleFittingEngine::setDeformationNeighborhoodSizes(const int& newLaplacianNeighborhoodSize, const int& newTransformationNeighborhoodSize, const int& newDeformationNeighborhoodSize) {
		    laplacianNeighborhoodSize      = newLaplacianNeighborhoodSize;
		    transformationNeighborhoodSize = newTransformationNeighborhoodSize;
		    deformationNeighborhoodSize    = newDeformationNeighborhoodSize;
		}

		MatrixFloat FlexibleFittingEngine::getDeformationTransformation(const int& vertexIndex) {
		    return deformationTransformations[vertexIndex];
		}

		void FlexibleFittingEngine::setDeformationVerticesToOriginalVertices() {
		    vertices = originalVertices;
		}

		Array2D<double> FlexibleFittingEngine::transformationForm(const Vec3F& vertex, const int& dimensions, const bool& oneFlag) {
		    vector<Vec3F> vertices;
		    vertices.push_back(vertex);

		    return transformationForm(vertices, dimensions, oneFlag);
		}

Array2D<double> FlexibleFittingEngine::transformationForm(const vector<Vec3F>& workVertices, const int& dimensions, const bool& oneFlag) {
    int numVertices = workVertices.size();
    
    Array2D<double> form(numVertices * dimensions, (2 * dimensions) + 1, 0.0);
    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        // X  0  Z -Y  1  0  0
        // Y -Z  0  X  0  1  0
        // Z  Y -X  0  0  0  1

        Vec3F vertex = workVertices[vertexIndex];

        form[vertexIndex * dimensions + 0][0] =  vertex.X();
        form[vertexIndex * dimensions + 0][2] =  vertex.Z();
        form[vertexIndex * dimensions + 0][3] = -vertex.Y();
       
        form[vertexIndex * dimensions + 1][0] =  vertex.Y();
        form[vertexIndex * dimensions + 1][1] = -vertex.Z();
        form[vertexIndex * dimensions + 1][3] =  vertex.X();
        
        form[vertexIndex * dimensions + 2][0] =  vertex.Z();
        form[vertexIndex * dimensions + 2][1] =  vertex.Y();
        form[vertexIndex * dimensions + 2][2] = -vertex.X();

        if (oneFlag) {
            form[vertexIndex * dimensions + 0][4] = 1.0;
            form[vertexIndex * dimensions + 1][5] = 1.0;
            form[vertexIndex * dimensions + 2][6] = 1.0;
        }
    }

    return form;
}

/**Array2D<double> FlexibleFittingEngine::inverse(const Array2D<double>& array) {
    if (array.dim1() != array.dim2()) {
        cerr << "Unable to invert matrix, not square!" << endl;
        exit(-1);
    }

    Array2D<double> identity(array.dim1(), array.dim1(), 0.0);
    for (int i = 0; i < array.dim1(); i++) {
        identity[i][i] = 1.0;
    }

    return linearSolve(array, identity);
}
**/
Array2D<double> FlexibleFittingEngine::transpose(const Array2D<double>& array) {
    Array2D<double> arrayTranspose(array.dim2(), array.dim1());

    for (int row = 0; row < array.dim1(); row++) {
        for (int col = 0; col < array.dim2(); col++) {
            arrayTranspose[col][row] = array[row][col];
        }
    }

    return arrayTranspose;
}

vector<Vec3F> FlexibleFittingEngine::GetSurfaceVertices(int noiseThreshold) {
	return mesh.GetSurfaceVertices(noiseThreshold);
}


vector<Vec3F> FlexibleFittingEngine::GetNonSurfaceVertices() {
	return mesh.GetNonSurfaceVertices();
}

//void FlexibleFittingEngine::addDeformationHandle(const int& vertexIndex, const Vec3F& targetVertex) {
  //  handleIndices.push_back(vertexIndex);
   // targetVertices.push_back(targetVertex);
//}

void FlexibleFittingEngine::resetDeformationHandles() {
    handleIndices.clear();
    targetVertices.clear();
}

void FlexibleFittingEngine::setDeformationVerticesToDeformedVertices() {
    vertices = deformedVertices;
}

void FlexibleFittingEngine::setDeformationWeights(const double& newFittingWeight, const double& newDistortionWeight) {
    fittingWeight    = newFittingWeight;
    distortionWeight = newDistortionWeight;
}

vector<Vec3F> FlexibleFittingEngine::getDeformedVertices() {
    return deformedVertices;
}

Array2D<double> FlexibleFittingEngine::linearSolve(const Array2D<double>& A, const Array2D<double>& B) {
    Array2D<double> solution;
    
    JAMA::LU<double> lu(A);
    //cout << A << endl;
    if (lu.isNonsingular()) {
        solution = lu.solve(B);
    }
    else {

        cerr << "Unable to solve linear system of equations using LU!" << endl;
        exit(-1);
    }

    return solution;
}

    /**double* FlexibleFittingEngine::sparseLinearSolve(const int& numVertices, const int& dimensions, const vector<int>& ATripletRowIndices, const vector<int>& ATripletColumnIndices, const vector<double>& ATripletValues, const vector<double>& B) {
        // Convert to UMFPACK Format
        int *AColumnCount      = new int [(numVertices * dimensions) + 1],
            *AColumnRowIndices = new int [ATripletValues.size()];

        double *AColumnValues = new double [ATripletValues.size()];

        umfpack_di_triplet_to_col(numVertices * dimensions, numVertices * dimensions, ATripletValues.size(), &ATripletRowIndices[0], &ATripletColumnIndices[0], &ATripletValues[0], AColumnCount, AColumnRowIndices, AColumnValues, NULL);

        // Solve
        void *symbolic,
             *numeric;
        
        double *X = new double [numVertices * dimensions];

        umfpack_di_symbolic(numVertices * dimensions, numVertices * dimensions, AColumnCount, AColumnRowIndices, AColumnValues, &symbolic, NULL, NULL);
        umfpack_di_numeric(AColumnCount, AColumnRowIndices, AColumnValues, symbolic, &numeric, NULL, NULL);
        umfpack_di_solve(UMFPACK_A, AColumnCount, AColumnRowIndices, AColumnValues, X, &B[0], numeric, NULL, NULL);

        // Cleanup
        delete [] AColumnCount;
        delete [] AColumnRowIndices;
        delete [] AColumnValues;
        umfpack_di_free_symbolic(&symbolic);
        umfpack_di_free_numeric(&numeric);

        return X;
    }**/
        void FlexibleFittingEngine::addDeformationRigidInitializationTransformation(const MatrixFloat& rigidInitializationTransformation) {
		    rigidInitializationTransformations.push_back(rigidInitializationTransformation);
		}


		void FlexibleFittingEngine::deformLaplacian() {
		    // Initialization
		    int numVertices = vertices.size(),
		        numHandles = handleIndices.size(),
		        dimensions = 3,
		        ataDim = numVertices * dimensions;

		    // Initialization for Eigen linear solver
		    //std::vector< Eigen::Triplet<double> > tripletList;
		    Eigen::MatrixXd ata = Eigen::MatrixXd::Constant(numVertices * dimensions, numVertices * dimensions, 0.0);
		    Eigen::VectorXd b;
		    b.setConstant(numVertices * dimensions, 0.0);

		    //map<int, double> ata;

		    //#if defined(_WIN32) || defined(_WIN64)
		        Array2D<double> AtA(numVertices * dimensions, numVertices * dimensions, 0.0),
		                        AtB(numVertices * dimensions, 1, 0.0);
		//    #else
		        //vector<int>    AtARowIndices,
		                    //   AtAColumnIndices;
		      //  vector<double> AtAValues,
		          //             AtB(numVertices * dimensions, 0.0);
		    //#endif

		    //cout << "Done initializing matrices!" << endl;
		   
		    // Fitting Terms
		    for (int handleIndex = 0; handleIndex < numHandles; handleIndex++) {
		        int vertexIndex = handleIndices[handleIndex];
		        Vec3F targetVertex = targetVertices[handleIndex];

		        for (int dimension = 0; dimension < dimensions; dimension++) {
		           // #if defined(_WIN32) || defined(_WIN64)
		                AtA[vertexIndex + (dimension * numVertices)][vertexIndex + (dimension * numVertices)] += fittingWeight * fittingWeight;
		                AtB[vertexIndex + (dimension * numVertices)][0] += fittingWeight * fittingWeight * targetVertex[dimension];
		            //#else
		               // AtARowIndices.push_back(vertexIndex + (dimension * numVertices));
		               // AtAColumnIndices.push_back(vertexIndex + (dimension * numVertices));
		               // AtAValues.push_back(fittingWeight * fittingWeight);
		               // AtB[vertexIndex + (dimension * numVertices)] += fittingWeight * fittingWeight * targetVertex[dimension];
		            //#endif

		            // For Eigen linear solver
		            //ata[(vertexIndex + (dimension * numVertices))*ataDim + vertexIndex + (dimension * numVertices)] += fittingWeight * fittingWeight;
		            ata(vertexIndex + dimension * numVertices, vertexIndex + dimension * numVertices) += fittingWeight * fittingWeight;
		            //tripletList.push_back(Eigen::Triplet<double>(vertexIndex + (dimension * numVertices), vertexIndex + (dimension * numVertices), fittingWeight * fittingWeight));
		            b[vertexIndex + (dimension * numVertices)] += fittingWeight * fittingWeight * targetVertex[dimension];
		        };
		    }
		    
		    //cout << "Done adding fitting terms!" << endl;
		    
		    //std::cout << "LP neighbor vertices: " << laplacianNeighborVertexIndicesCache[handleIndices[numHandles/2]].size() << "\n";
		    //std::cout << "Trans neighbor vertices: " << transformationNeighborVertexIndicesCache[handleIndices[numHandles/2]].size() << "\n";

		    // Distortion Terms
		    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
		        // Lookup Rigid Initialization Transformation
		        MatrixFloat rigidInitializationTransformation(4, 4);
		        if (rigidInitialization) {
		            rigidInitializationTransformation = rigidInitializationTransformations[vertexIndex];
		        }


		        // Lookup Vertex
		        Vec3F vertex = vertices[vertexIndex]; 
		        if (rigidInitialization) {
		            vertex = vertex.transform(rigidInitializationTransformation);
		        }

		        // Calculate Laplacian
		        vector<int> laplacianNeighborVertexIndices = laplacianNeighborVertexIndicesCache[vertexIndex];
		        int laplacianNumNeighbors = laplacianNeighborVertexIndices.size();
		        if (laplacianNumNeighbors == 0) {
		            cerr << "Unable to calculate laplacian, vertex with 0 neighbors!" << endl;
		            exit(-1);
		        }

		        Vec3F neighborSum(0.0, 0.0, 0.0);
		        for (vector<int>::iterator neighborVertexIndexIterator = laplacianNeighborVertexIndices.begin(); neighborVertexIndexIterator != laplacianNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
		            Vec3F neighborVertex = vertices[*neighborVertexIndexIterator];
		            if (rigidInitialization) {
		                neighborVertex = neighborVertex.transform(rigidInitializationTransformation);
		            }
		            neighborSum += neighborVertex;
		        }
		        Vec3F laplacian = vertex - (neighborSum * (1.0 / laplacianNumNeighbors));
		  
		        // Transformation Compensation
		        vector<int> transformationNeighborVertexIndices = transformationNeighborVertexIndicesCache[vertexIndex];
		        //if(transformationNeighborVertexIndices.size() != 3) {
		        //	continue;
		        //}
		        vector<Vec3F> transformationNeighborhoodVertices(1, vertex);
		        for (vector<int>::iterator neighborVertexIndexIterator = transformationNeighborVertexIndices.begin(); neighborVertexIndexIterator != transformationNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
		            Vec3F neighborVertex = vertices[*neighborVertexIndexIterator];
		            if (rigidInitialization) {
		                neighborVertex = neighborVertex.transform(rigidInitializationTransformation);
		            }
		            transformationNeighborhoodVertices.push_back(neighborVertex);
		        }
		        
		        Array2D<double> C  = transformationForm(transformationNeighborhoodVertices, dimensions, true),
		                        D  = transformationForm(laplacian, dimensions, false),
		                        Ct = transpose(C);
		        Array2D<double> CtC = matmult(Ct, C);

		        Eigen::MatrixXd cTc(CtC.dim1(), CtC.dim2());
		        for(int i = 0; i < CtC.dim1(); i++) {
		        	for(int j = 0; j < CtC.dim2(); j++) {
		        		cTc(i, j) = CtC[i][j];
		        	}
		        }
		        Eigen::MatrixXd cTcInverse = cTc.inverse();
		        Array2D<double> cTcInverseArray(CtC.dim1(), CtC.dim2());
		        for(int i = 0; i < CtC.dim1(); i++) {
		        	for(int j = 0; j < CtC.dim2(); j++) {
		        		cTcInverseArray[i][j] = cTcInverse(i,j);
		        	}
		        }
		        Array2D<double> T  = matmult(matmult(D, cTcInverseArray), Ct);

		        // Construct Row
		        for (int dimensionOuter = 0; dimensionOuter < dimensions; dimensionOuter++) {
		            map<int, double> a;
		            
		            a[vertexIndex + (dimensionOuter * numVertices)] += distortionWeight;
		            for (int dimensionInner = 0; dimensionInner < dimensions; dimensionInner++) {
		                a[vertexIndex + (dimensionInner * numVertices)] -= distortionWeight * T[dimensionOuter][dimensionInner];
		            }
		           
		            for (vector<int>::iterator neighborVertexIndexIterator = laplacianNeighborVertexIndices.begin(); neighborVertexIndexIterator != laplacianNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
		                int neighborVertexIndex = *neighborVertexIndexIterator;
		                
		                a[neighborVertexIndex + (dimensionOuter * numVertices)] -= distortionWeight / laplacianNumNeighbors;
		            }

		            int neighborIndex = 1;
		            for (vector<int>::iterator neighborVertexIndexIterator = transformationNeighborVertexIndices.begin(); neighborVertexIndexIterator != transformationNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
		                int neighborVertexIndex = *neighborVertexIndexIterator;

		                for (int dimensionInner = 0; dimensionInner < dimensions; dimensionInner++) {
		                    a[neighborVertexIndex + (dimensionInner * numVertices)] -= distortionWeight * T[dimensionOuter][(neighborIndex * dimensions) + dimensionInner];
		                }
		                
		                neighborIndex++;
		            }
		            
		            for (map<int, double>::iterator atIterator = a.begin(); atIterator != a.end(); atIterator++) {
		                //cout << "\n";
		                for (map<int, double>::iterator aIterator = a.begin(); aIterator != a.end(); aIterator++) {
		                    int atIndex = (*atIterator).first,
		                        aIndex  = (*aIterator).first;
		            
		                    double atValue = (*atIterator).second,
		                           aValue  = (*aIterator).second;
		           
		                   // #if defined(_WIN32) || defined(_WIN64)
		                        AtA[atIndex][aIndex] += atValue * aValue;
		                   // #else
		                        //AtARowIndices.push_back(atIndex);
		                       // AtAColumnIndices.push_back(aIndex);
		                       // AtAValues.push_back(atValue * aValue);
		                   // #endif
		                    //tripletList.push_back(Eigen::Triplet<double>(atIndex, aIndex, atValue * aValue));
		                    //ata[atIndex*ataDim + aIndex] += atValue * aValue;
		                    ata(atIndex, aIndex) += atValue * aValue;
		                    //cout << (*atIterator).first << ", ";
		                }
		                //tripletList.push_back(Eigen::Triplet<double>(atIndex, aIndex, atValue * aValue));
		            }
		            //cout << "\n";
		        }
		    }

		    //cout << "Done adding distortion terms!" << endl;
		    //cout << "AtA fill rate: " << ata.size() << ", " << (numVertices * dimensions * numVertices * dimensions) << ", " << (float)ata.size() / (float)(numVertices * dimensions * numVertices * dimensions) << endl;

		    // Solution
		    /// Eigen sparse
		    //Eigen::SparseMatrix<double, Eigen::ColMajor> A(numVertices * dimensions, numVertices * dimensions);
		    //for (map<int, double>::iterator it = ata.begin(); it != ata.end(); ++it)
		    //{
		    //    tripletList.push_back(Eigen::Triplet<double>(it->first/ataDim, it->first % ataDim, it->second));
		    //}    
		    //A.setFromTriplets(tripletList.begin(), tripletList.end());

		    //// Set up "x" for Ax=b
		    //Eigen::VectorXd x(numVertices * dimensions);
		    //// Setup solver
		    //Eigen::SparseLU< Eigen::SparseMatrix<double, Eigen::ColMajor>, Eigen::COLAMDOrdering<int> > solver;
		    ////Eigen::BiCGSTAB <Eigen::SparseMatrix<double, Eigen::ColMajor>> solver;

		    //// Solve for Ax=b
		    //A.makeCompressed();
		    //solver.compute(A);
		    //x = solver.solve(b);

		    //if (solver.info() != Eigen::Success)
		    //{
		    //    // solving failed
		    //    std::cout << "Eigen sparse solve AtAx=AtB fail...\n";
		    //}
		    //// Eigen dense

		    Eigen::VectorXd x = ata.ldlt().solve(b); // ata.ldlt().solve(b) ata.llt().solve(b)

		    //#if defined(_WIN32) || defined(_WIN64)
		        //Array2D<double> X = linearSolve(AtA, AtB);
		   // #else
		       //double* X = sparseLinearSolve(numVertices, dimensions, AtARowIndices, AtAColumnIndices, AtAValues, AtB);
		   //#endif

		    //cout << "Done solving linear system!" << endl;

		    // Convert to Vector
		    deformedVertices.clear();
		    deformedVertices.reserve(numVertices);
		    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
		       // #if defined(_WIN32) || defined(_WIN64)
		            //deformedVertices.push_back(Vec3F(
		              //  X[(vertexIndex + (0 * numVertices))][0], 
		               // X[(vertexIndex + (1 * numVertices))][0], 
		               // X[(vertexIndex + (2 * numVertices))][0]
		            //));
		       // #else
		          // deformedVertices.push_back(Vec3F(
		           //     X[(vertexIndex + (0 * numVertices))], 
		           //     X[(vertexIndex + (1 * numVertices))], 
		           //     X[(vertexIndex + (2 * numVertices))]
		           // ));
		       // #endif
		        deformedVertices.push_back(Vec3F(
		            x[(vertexIndex + (0 * numVertices))], 
		            x[(vertexIndex + (1 * numVertices))], 
		            x[(vertexIndex + (2 * numVertices))]
		        ));
		    }

		    // Compare the deformed vertices with vertices (by rmsd)
		    m_shapeDistance = 0.0;
		    for (int i = 0; i < numVertices; ++i)
		    {
		        m_shapeDistance += (deformedVertices[i][0] - vertices[i][0])*(deformedVertices[i][0] - vertices[i][0]) +
		                         (deformedVertices[i][1]-vertices[i][1])*(deformedVertices[i][1]-vertices[i][1]) + 
		                         (deformedVertices[i][2]-vertices[i][2])*(deformedVertices[i][2]-vertices[i][2]);
		    }
		    m_shapeDistance = sqrt(m_shapeDistance / (double)numVertices);

		    //cout << "RMSD compared to last iteration: "<< shapeDistance << endl;
		    
		    // Cleanup
		    //#if !defined(_WIN32) && !defined(_WIN64)
		      //  delete[] X;
		    //#endif
		}


		void FlexibleFittingEngine::AddCorrespondence(int alignmentIx, int SSEIx, int PDBIx){
			//Creates a correspondence between SSE elements SSEIx and PDBIx, removing them from their 
			//current cluster and placing them in their own.
			vector < vector<SSECorrespondenceNode> >  newCorrs;		
			//loop through every cluster
			for(unsigned int i = 0; i < corrs[alignmentIx].size(); ++i){
				if(corrs[alignmentIx][i].size() > 1){
					newCorrs.push_back(vector<SSECorrespondenceNode>());
					for(unsigned int j = 0; j < corrs[alignmentIx][i].size(); ++j){
						if(corrs[alignmentIx][i][j].GetPIndex() != PDBIx && corrs[alignmentIx][i][j].GetQIndex() != SSEIx){
							newCorrs[newCorrs.size()-1].push_back(corrs[alignmentIx][i][j]);
						}
					}
				}
				//size of one is a boundary case if we have to not copy that element
				else if(corrs[alignmentIx][i].size() == 1){
					if(corrs[alignmentIx][i][0].GetPIndex() != PDBIx && corrs[alignmentIx][i][0].GetQIndex() != SSEIx){
						newCorrs.push_back(vector<SSECorrespondenceNode>());
						newCorrs[newCorrs.size()-1].push_back(corrs[alignmentIx][i][0]);
					}
				}
			}

			newCorrs.push_back(vector<SSECorrespondenceNode>());
			newCorrs[newCorrs.size()-1].push_back(SSECorrespondenceNode(PDBIx, SSEIx, true));//TODO <--------- FIX THIS (last paramater)
			corrs[alignmentIx].clear();
			corrs[alignmentIx] = newCorrs;

		}

		void FlexibleFittingEngine::MergeClusters(int alignmentIx, int cluster1, int cluster2){
			//Merges clusters given their index in corrs

			vector < vector<SSECorrespondenceNode> >  newCorrs;

			//creating new cluster
			vector<SSECorrespondenceNode> newCluster;
			for(unsigned int i = 0; i < corrs[alignmentIx][cluster1].size(); ++i){
				newCluster.push_back(corrs[alignmentIx][cluster1][i]);
			}
			for(unsigned int i = 0; i < corrs[alignmentIx][cluster2].size(); ++i){
				newCluster.push_back(corrs[alignmentIx][cluster2][i]);
			}
			
			//copying old clusters.
			for(unsigned int i = 0; i < corrs[alignmentIx].size(); ++i){
				if(i != cluster1 && i != cluster2){
					newCorrs.push_back(corrs[alignmentIx][i]);
				}else if(i == cluster1){//new cluster retains same index as cluster1
					newCorrs.push_back(newCluster);
				}
			}
			
			corrs[alignmentIx].clear();
			corrs[alignmentIx] = newCorrs;
		}

		int FlexibleFittingEngine::GetCorrespondenceCount() {
			return corrs.size();
		}

		int FlexibleFittingEngine::GetClusterCount(int corrIx) {
			return corrs[corrIx].size();
		}

		int FlexibleFittingEngine::GetHelixCount(int corrIx, int clusterIx) {
			return corrs[corrIx][clusterIx].size();
		}

		/**MatrixFloat FlexibleFittingEngine::getCorrespondenceFeatureTransformation(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex) {
		    // Get Correspondence
		    SSECorrespondenceNode correspondence = getCorrespondence(alignmentIndex, clusterIndex, featureIndex);

		    int sourceFeatureIndex = correspondence.GetPIndex(),
		        targetFeatureIndex = correspondence.GetQIndex();
		    
		    bool flip = !correspondence.IsForward();

		    // Cluster Transformation
		    MatrixFloat clusterTransformation = getCorrespondenceClusterTransformation(alignmentIndex, clusterIndex);

		    // Translation
		    Vec3F sourceCentroid    = sourceFeatures[sourceFeatureIndex].GetCentroid().transform(clusterTransformation),
		                  targetCentroid    = targetFeatures[targetFeatureIndex].GetCentroid(),
		                  translationVector = targetCentroid - sourceCentroid;

		    MatrixFloat translationTransformation = MatrixFloat::Identity(4);
		    translationTransformation.SetValue(translationVector[0], 0, 3);
		    translationTransformation.SetValue(translationVector[1], 1, 3);
		    translationTransformation.SetValue(translationVector[2], 2, 3);

		    // Rotation
		    Vec3F sourceStartPosition = sourceFeatures[sourceFeatureIndex].GetEndPoint(0).transform(translationTransformation * clusterTransformation),
		                  sourceEndPosition   = sourceFeatures[sourceFeatureIndex].GetEndPoint(1).transform(translationTransformation * clusterTransformation),
		                  targetStartPosition = targetFeatures[targetFeatureIndex].GetEndPoint(0),
		                  targetEndPosition   = targetFeatures[targetFeatureIndex].GetEndPoint(1),
		                  sourceVector        = sourceEndPosition - sourceStartPosition,
		                  targetVector        = targetEndPosition - targetStartPosition;

		    sourceVector.normalize();
		    targetVector.normalize();
		    
		    Vec3F normal = sourceVector ^ targetVector;
		    normal.normalize();

		    double angle = acos(sourceVector * targetVector);
		    if (angle > (PI / 2)) {
		        angle -= PI;
		    }
		    if (flip) {
		        angle *= -1;
		    }

		    Vec3F row0(1.0, 0.0, 0.0),
		                  row1(0.0, 1.0, 0.0),
		                  row2(0.0, 0.0, 1.0);

		    row0.rotate(normal, angle);
		    row1.rotate(normal, angle);
		    row2.rotate(normal, angle);

		    MatrixFloat rotationTransformation = MatrixFloat::Identity(4);
		    rotationTransformation.SetValue(row0[0], 0, 0); rotationTransformation.SetValue(row0[1], 0, 1); rotationTransformation.SetValue(row0[2], 0, 2);
		    rotationTransformation.SetValue(row1[0], 1, 0); rotationTransformation.SetValue(row1[1], 1, 1); rotationTransformation.SetValue(row1[2], 1, 2);
		    rotationTransformation.SetValue(row2[0], 2, 0); rotationTransformation.SetValue(row2[1], 2, 1); rotationTransformation.SetValue(row2[2], 2, 2);

		    // Rotation Origin Offset
		    sourceCentroid = sourceFeatures[sourceFeatureIndex].GetCentroid().Transform(translationTransformation * clusterTransformation);
		    
		    MatrixFloat translationToOriginTransformation = MatrixFloat::Identity(4);
		    translationToOriginTransformation.SetValue(-sourceCentroid[0], 0, 3);
		    translationToOriginTransformation.SetValue(-sourceCentroid[1], 1, 3);
		    translationToOriginTransformation.SetValue(-sourceCentroid[2], 2, 3);

		    MatrixFloat translationFromOriginTransformation = MatrixFloat::Identity(4);
		    translationFromOriginTransformation.SetValue(sourceCentroid[0], 0, 3);
		    translationFromOriginTransformation.SetValue(sourceCentroid[1], 1, 3);
		    translationFromOriginTransformation.SetValue(sourceCentroid[2], 2, 3);

		    return translationFromOriginTransformation * rotationTransformation * translationToOriginTransformation * translationTransformation * clusterTransformation;
		}**/

		SSECorrespondenceNode FlexibleFittingEngine::GetCorrespondenceNode(int corrIx, int clusterIx, int helixIx) {
			return corrs[corrIx][clusterIx][helixIx];
		}

		int FlexibleFittingEngine::getCorrespondenceFeatureCount(const int& alignmentIndex, const int& clusterIndex) {
		    return corrs[alignmentIndex][clusterIndex].size();
		}

		void FlexibleFittingEngine::addCorrespondenceSourceFeature(const Vec3F& startVertex, const Vec3F& endVertex) {
		    sourceFeatures.push_back(SSECorrespondenceFeature(startVertex, endVertex));
		}

		void FlexibleFittingEngine::addCorrespondenceTargetFeature(const Vec3F& startVertex, const Vec3F& endVertex) {
		    targetFeatures.push_back(SSECorrespondenceFeature(startVertex, endVertex));
		}

		void FlexibleFittingEngine::loadHelixFileRenderer(string fileName) {
			//cout << "in load function" << endl;
 			sse_renderer->loadHelixFile(fileName);
 			//cout << "finished loading in c++" << endl;
		}

		void FlexibleFittingEngine::calculateNeighborVertexIndicesCaches(const int& numVertices) {
		    // Initialize Immediate Neighbor Cache
		    vector< vector<int> > neighborVertexIndicesCache(numVertices, vector<int>());

		    // Calculate Immediate Neighbor Cache
		    for (vector< pair<int, int> >::iterator edgeIterator = edges.begin(); edgeIterator != edges.end(); edgeIterator++) {
		        pair<int, int> edge = *edgeIterator;
		    
		        neighborVertexIndicesCache[edge.first].push_back(edge.second);
		        neighborVertexIndicesCache[edge.second].push_back(edge.first);
		    }

		    // Calculate Extended Neighbor Caches
		    laplacianNeighborVertexIndicesCache      = vector< vector<int> >(numVertices, vector<int>());
		    transformationNeighborVertexIndicesCache = vector< vector<int> >(numVertices, vector<int>());
		    deformationNeighborVertexIndicesCache    = vector< vector<int> >(numVertices, vector<int>());
		    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
		        queue< pair<int, int> > frontier;
		        set<int> explored;

		        frontier.push(make_pair(vertexIndex, 0));
		        explored.insert(vertexIndex);
		        while (!frontier.empty()) {
		            pair<int, int> exploration = frontier.front();

		            int explorationVertexIndex = exploration.first,
		                explorationDistance    = exploration.second;

		            if (explorationVertexIndex != vertexIndex) {
		                if (explorationDistance <= laplacianNeighborhoodSize) {
		                    laplacianNeighborVertexIndicesCache[vertexIndex].push_back(explorationVertexIndex);
		                }

		                if (explorationDistance <= transformationNeighborhoodSize) {
		                    transformationNeighborVertexIndicesCache[vertexIndex].push_back(explorationVertexIndex);
		                }

		                if (explorationDistance <= deformationNeighborhoodSize) {
		                    deformationNeighborVertexIndicesCache[vertexIndex].push_back(explorationVertexIndex);
		                }

		                if (explorationDistance > laplacianNeighborhoodSize && explorationDistance > transformationNeighborhoodSize && explorationDistance > deformationNeighborhoodSize) {
		                    break;
		                }
		            }
		            frontier.pop();

		            vector<int> neighborVertexIndices = neighborVertexIndicesCache[explorationVertexIndex];
		            for (vector<int>::iterator neighborVertexIndexIterator = neighborVertexIndices.begin(); neighborVertexIndexIterator != neighborVertexIndices.end(); neighborVertexIndexIterator++) {
		                int neighborVertexIndex = *neighborVertexIndexIterator;

		                if (explored.count(neighborVertexIndex) == 0) {
		                    frontier.push(make_pair(neighborVertexIndex, explorationDistance + 1));
		                    explored.insert(neighborVertexIndex);
		                }
		            }
		        }
		        
		        sort(laplacianNeighborVertexIndicesCache[vertexIndex].begin(), laplacianNeighborVertexIndicesCache[vertexIndex].end());
		        sort(transformationNeighborVertexIndicesCache[vertexIndex].begin(), transformationNeighborVertexIndicesCache[vertexIndex].end());
		        sort(deformationNeighborVertexIndicesCache[vertexIndex].begin(), deformationNeighborVertexIndicesCache[vertexIndex].end());
		    }

		    //cout << "Done calculating neighbor vertex indices caches!" << endl;
		}


		MatrixFloat FlexibleFittingEngine::GetRigidTransform(int corrIx) {
			int maxIndex = 0;
			for(unsigned int i = 1; i < corrs[corrIx].size(); i++) {
				if(corrs[corrIx][i].size() > corrs[corrIx][maxIndex].size()) {
					maxIndex = i;
				}
			}
			return finder.GetTransform(corrs[corrIx][maxIndex], SAMPLE_COUNT);			
		}

		MatrixFloat FlexibleFittingEngine::getCorrespondenceClusterTransformation(const int& alignmentIndex, const int& clusterIndex) {
		    vector<SSECorrespondenceNode> clusterCorrespondences;
		    //cout << "in cluster 0" << endl;
		    if (getCorrespondenceFeatureCount(alignmentIndex, clusterIndex) == 1) {
		        // Get Correspondence
		        SSECorrespondenceNode correspondence = GetCorrespondenceNode(alignmentIndex, clusterIndex, 0);
		        //cout << "in cluster 1" << endl;
		        int sourceFeatureIndex = correspondence.GetPIndex(),
		            targetFeatureIndex = correspondence.GetQIndex();
		 
		        // Get Source Feature Centroid
		        Vec3F sourceFeatureCentroid = sourceFeatures[sourceFeatureIndex].GetCentroid();
		        //cout << "in cluster 2" << endl;

		        // Calculate Distances to Other Source Features
				vector< boost::tuple<double, int, int> > closestSourceFeatures;
		        for (int otherClusterIndex = 0; otherClusterIndex < GetClusterCount(alignmentIndex); otherClusterIndex++) {
		            if (otherClusterIndex == clusterIndex) {
		                continue;
		            }
		            //cout << "in cluster 3" << endl;

		            for (int otherFeatureIndex = 0; otherFeatureIndex < getCorrespondenceFeatureCount(alignmentIndex, otherClusterIndex); otherFeatureIndex++) {
		                // Get Other Correspondence
		                //cout << "in cluster 3.5" << endl;
		                SSECorrespondenceNode otherCorrespondence = GetCorrespondenceNode(alignmentIndex, otherClusterIndex, otherFeatureIndex);
		                //cout << "in cluster 4" << endl;
		                // Get Other Source Feature Index
		                int otherSourceFeatureIndex = otherCorrespondence.GetPIndex();
		                
		                // Get Other Source Feature Centroid
		                Vec3F otherSourceFeatureCentroid = sourceFeatures[otherSourceFeatureIndex].GetCentroid();

		                // Calculate Distance
		                double distance = (otherSourceFeatureCentroid - sourceFeatureCentroid).length();

		                // Store Distance and Correspondence
						closestSourceFeatures.push_back(boost::tuple<double, int, int>(distance, otherClusterIndex, otherFeatureIndex));
		            }
		        }
		       // cout << "in cluster 5" << endl;
		        // Sort Distances 
		        sort(closestSourceFeatures.begin(), closestSourceFeatures.end());

		        // Build Temporary Cluster
		        int clusterCorrespondencesSize = 3;

		        for (int clusterCorrespondenceIndex = 0; clusterCorrespondenceIndex < clusterCorrespondencesSize; clusterCorrespondenceIndex++) {
		            double distance       = closestSourceFeatures[clusterCorrespondenceIndex].get<0>();
		           // cout << "in cluster 6" << endl;
		            int otherClusterIndex = closestSourceFeatures[clusterCorrespondenceIndex].get<1>(),
		                otherFeatureIndex = closestSourceFeatures[clusterCorrespondenceIndex].get<2>();
		            //cout << "in cluster 7" << endl;
		            clusterCorrespondences.push_back(GetCorrespondenceNode(alignmentIndex, otherClusterIndex, otherFeatureIndex));
		        	//cout << "in cluster 8" << endl;
		        }
		    }
		    else {
		    	//cout << "in cluster 8.5" << endl;
		        clusterCorrespondences = corrs[alignmentIndex][clusterIndex];
		        //cout << "in cluster 9" << endl;
		    }
		    //cout << "in cluster 9.5" << endl;
		    MatrixFloat clusterTransformation = finder.GetTransform(clusterCorrespondences, 10);
		    //cout << "in cluster 10" << endl;
		    return clusterTransformation;
		}

		MatrixFloat FlexibleFittingEngine::getCorrespondenceFeatureTransformation(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex) {
		    // Get Correspondence
		    SSECorrespondenceNode correspondence = GetCorrespondenceNode(alignmentIndex, clusterIndex, featureIndex);
		    int sourceFeatureIndex = correspondence.GetPIndex(),
		        targetFeatureIndex = correspondence.GetQIndex();
		    bool flip = !correspondence.IsForward();
		    // Cluster Transformation
		    MatrixFloat clusterTransformation = getCorrespondenceClusterTransformation(alignmentIndex, clusterIndex);
		    // Translation
		    Vec3F sourceCentroid    = sourceFeatures[sourceFeatureIndex].GetCentroid().transform(clusterTransformation),
		                  targetCentroid    = targetFeatures[targetFeatureIndex].GetCentroid(),
		                  translationVector = targetCentroid - sourceCentroid;
		    MatrixFloat translationTransformation = MatrixFloat::Identity(4);
		    translationTransformation.SetValue(translationVector[0], 0, 3);
		    translationTransformation.SetValue(translationVector[1], 1, 3);
		    translationTransformation.SetValue(translationVector[2], 2, 3);

		    // Rotation
		    Vec3F sourceStartPosition = sourceFeatures[sourceFeatureIndex].GetEndPoint(0).transform(translationTransformation * clusterTransformation),
		                  sourceEndPosition   = sourceFeatures[sourceFeatureIndex].GetEndPoint(1).transform(translationTransformation * clusterTransformation),
		                  targetStartPosition = targetFeatures[targetFeatureIndex].GetEndPoint(0),
		                  targetEndPosition   = targetFeatures[targetFeatureIndex].GetEndPoint(1),
		                  sourceVector        = sourceEndPosition - sourceStartPosition,
		                  targetVector        = targetEndPosition - targetStartPosition;

		    sourceVector.normalize();
		    targetVector.normalize();
		    
		    Vec3F normal = sourceVector ^ targetVector;
		    normal.normalize();

		    double angle = acos(sourceVector * targetVector);
		    if (angle > (PI / 2)) {
		        angle -= PI;
		    }
		    if (flip) {
		        angle *= -1;
		    }

		    Vec3F row0(1.0, 0.0, 0.0),
		                  row1(0.0, 1.0, 0.0),
		                  row2(0.0, 0.0, 1.0);

		    row0.rotate(normal, angle);
		    row1.rotate(normal, angle);
		    row2.rotate(normal, angle);

		    MatrixFloat rotationTransformation = MatrixFloat::Identity(4);
		    rotationTransformation.SetValue(row0[0], 0, 0); rotationTransformation.SetValue(row0[1], 0, 1); rotationTransformation.SetValue(row0[2], 0, 2);
		    rotationTransformation.SetValue(row1[0], 1, 0); rotationTransformation.SetValue(row1[1], 1, 1); rotationTransformation.SetValue(row1[2], 1, 2);
		    rotationTransformation.SetValue(row2[0], 2, 0); rotationTransformation.SetValue(row2[1], 2, 1); rotationTransformation.SetValue(row2[2], 2, 2);

		    // Rotation Origin Offset
		    sourceCentroid = sourceFeatures[sourceFeatureIndex].GetCentroid().transform(translationTransformation * clusterTransformation);
		    
		    MatrixFloat translationToOriginTransformation = MatrixFloat::Identity(4);
		    translationToOriginTransformation.SetValue(-sourceCentroid[0], 0, 3);
		    translationToOriginTransformation.SetValue(-sourceCentroid[1], 1, 3);
		    translationToOriginTransformation.SetValue(-sourceCentroid[2], 2, 3);

		    MatrixFloat translationFromOriginTransformation = MatrixFloat::Identity(4);
		    translationFromOriginTransformation.SetValue(sourceCentroid[0], 0, 3);
		    translationFromOriginTransformation.SetValue(sourceCentroid[1], 1, 3);
		    translationFromOriginTransformation.SetValue(sourceCentroid[2], 2, 3);
		    return translationFromOriginTransformation * rotationTransformation * translationToOriginTransformation * translationTransformation * clusterTransformation;
		}

		//Gets a rigid transform given the alignment we are working with and the cluster #
		MatrixFloat FlexibleFittingEngine::GetRigidTransform2(int corrIx, int clusterIx) {
			//If of size one we can handle it
			if(corrs[corrIx][clusterIx].size() > 1){
				return finder.GetTransform(corrs[corrIx][clusterIx], SAMPLE_COUNT);	
			}
			else{
				vector<boost::tuples::tuple<int, int> > PQMatchings;
				if(helixDistancesSorted.size() == 0){
					BuildSortedHelixDistances(corrIx);
				}			
				
				int helixIx = corrs[corrIx][clusterIx][0].GetPIndex();//index of 0 because only one element
				
				//creating a temporary cluster so we can use SSECorrespondenceFinder to get transform
				vector<SSECorrespondenceNode> tempCluster;
				int ind1 = helixDistancesSorted[helixIx].get<0>();
				int ind2 = helixDistancesSorted[helixIx].get<1>();
				int ind3 = helixDistancesSorted[helixIx].get<2>();
				for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
					for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
						int p = corrs[corrIx][i][j].GetPIndex();
						if(p == ind1 || p == ind2 || p == ind3 || p == helixIx){
							tempCluster.push_back(corrs[corrIx][i][j]);
						}
					}
				}

				return finder.GetTransform(tempCluster, SAMPLE_COUNT);
			}
		}

		MatrixFloat FlexibleFittingEngine::GetSideChainTransform(vector<Vec3F> origLocations, vector<Vec3F> newLocations){
			/*cout << "Going from " ; origLocations[0].Print();			
			cout << " and "; origLocations[origLocations.size()-1].Print();
			cout << "Going to " ; newLocations[0].Print();			
			cout << " and "; newLocations[newLocations.size()-1].Print(); cout << endl;*/
			return finder.GetTransform(origLocations, newLocations, SAMPLE_COUNT);
		}

		//finds a transform to line a PDB helix up exactly with an SSE Helix
		MatrixFloat FlexibleFittingEngine::GetHelixFlexibleTransform(int corrIx, int helixIx) {
			int corrNum = -1;
			int qIndex = -1;
			bool flip = false;

			//These will be combined into final transformation
			MatrixFloat clusterTrans = MatrixFloat(4,4);
			MatrixFloat translationTrans = MatrixFloat(4,4);
			MatrixFloat rotationTrans = MatrixFloat(4,4);
			MatrixFloat negCentroidTrans = MatrixFloat(4,4);
			MatrixFloat posCentroidTrans= MatrixFloat(4,4);

			//obtaining relevant indices and info
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						qIndex = corrs[corrIx][i][j].GetQIndex();
						flip = !corrs[corrIx][i][j].IsForward();
						if(corrs[corrIx][i].size() > 1){
							clusterTrans = finder.GetTransform(corrs[corrIx][i], SAMPLE_COUNT);
						}
						else{
							corrNum = i;
							break;
						}
					}
				}
			}
			//if we didnt find cluster or the cluster transform is bad return identity matrix
			if((corrNum == -1 && clusterTrans.GetValue(3,3) < 0.5) || qIndex == -1){ //doesn't have a correspondence
				MatrixFloat result = MatrixFloat(4,4);
				return result;
			}

			if(helixDistancesSorted.size() == 0){
				BuildSortedHelixDistances(corrIx);
			}
			
			//cluster of size one, have to build a temp cluster and find its transform
			if(clusterTrans.GetValue(3,3) == 0.0){
				vector<SSECorrespondenceNode> tempCluster;
				int ind1 = helixDistancesSorted[helixIx].get<0>();
				int ind2 = helixDistancesSorted[helixIx].get<1>();
				int ind3 = helixDistancesSorted[helixIx].get<2>();
				for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
					for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
						int p = corrs[corrIx][i][j].GetPIndex();
						if(p == ind1 || p == ind2 || p == ind3 || p == helixIx){
							tempCluster.push_back(corrs[corrIx][i][j]);
						}
					}
				}
				clusterTrans =  finder.GetTransform(tempCluster, SAMPLE_COUNT);
			}
			

			//need the translation between centroid and origin in order to do a rotation
			Vec3F beforeTrans = featureList1[helixIx].GetCentroid();
			Vec3F center1 = beforeTrans.transform(clusterTrans);
			Vec3F center2 = featureList2[qIndex].GetCentroid();
			Vec3F diff = center2 - center1;			
			translationTrans.SetValue(diff[0], 0, 3);	translationTrans.SetValue(diff[1], 1, 3);	translationTrans.SetValue(diff[2], 2, 3);
			translationTrans.SetValue(1,0,0);	translationTrans.SetValue(1,1,1);	translationTrans.SetValue(1,2,2);	translationTrans.SetValue(1,3,3);


			//applying the transforms so far in order to get correct rotation trans
			MatrixFloat trans = translationTrans * clusterTrans;

			//Calculating rotation transform
			Vec3F end1 = featureList1[helixIx].GetEndPoint(0).transform(trans);
			Vec3F end2 = featureList1[helixIx].GetEndPoint(1).transform(trans);
			Vec3F featVec1 = end2 - end1;
			Vec3F featVec2 = featureList2[qIndex].GetFeatureVector();
			featVec1.normalize();
			featVec2.normalize();
			Vec3F normal = featVec1^featVec2; //cross product
			normal.normalize();
			double angle = acos(featVec1*featVec2);

			if(angle > PI/2)
				angle = PI - angle;
			if(flip)
				angle = angle*-1;
			//R is desired rotation matrix, A is original locations, B is locations after rotation
			//A*R = B
			//R = A^-1*B
			//A = I
			//R = B
			Vec3F row1 = Vec3F(1.0,0.0,0.0);	row1 = row1.rotate(normal,angle);
			Vec3F row2 = Vec3F(0.0,1.0,0.0);	row2 = row2.rotate(normal,angle);
			Vec3F row3 = Vec3F(0.0,0.0,1.0);	row3 = row3.rotate(normal,angle);
			rotationTrans.SetValue(row1[0],0,0);	rotationTrans.SetValue(row1[1],0,1);	rotationTrans.SetValue(row1[2],0,2);			
			rotationTrans.SetValue(row2[0],1,0);	rotationTrans.SetValue(row2[1],1,1);	rotationTrans.SetValue(row2[2],1,2);			
			rotationTrans.SetValue(row3[0],2,0);	rotationTrans.SetValue(row3[1],2,1);	rotationTrans.SetValue(row3[2],2,2);			
			rotationTrans.SetValue(1,3,3);

			//Calculating transform back from origin to original location
			Vec3F centroid = (end1+end2)*0.5;
			negCentroidTrans.SetValue(-centroid[0],0,3);	negCentroidTrans.SetValue(-centroid[1],1,3);	negCentroidTrans.SetValue(-centroid[2],2,3);
			negCentroidTrans.SetValue(1,0,0);	negCentroidTrans.SetValue(1,1,1);	negCentroidTrans.SetValue(1,2,2);	negCentroidTrans.SetValue(1,3,3);

			posCentroidTrans.SetValue(centroid[0],0,3);	posCentroidTrans.SetValue(centroid[1],1,3);	posCentroidTrans.SetValue(centroid[2],2,3);
			posCentroidTrans.SetValue(1,0,0);	posCentroidTrans.SetValue(1,1,1);	posCentroidTrans.SetValue(1,2,2);	posCentroidTrans.SetValue(1,3,3);

			//Multiply them in the reverse order that they would have been applied to a point to get final matrix
			return posCentroidTrans*rotationTrans*negCentroidTrans*translationTrans*clusterTrans;
		}

		//for each pdb helix finds and stores the 3 closest pdb helices by centroid
		//stored in helixDistancesSorted
		void FlexibleFittingEngine::BuildSortedHelixDistances(int corrIx){
			vector<boost::tuples::tuple<int, int> > PQMatchings;
			
			//saving which SSE elements correspond
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					PQMatchings.push_back(boost::tuples::tuple<int, int>(corrs[corrIx][i][j].GetPIndex(), corrs[corrIx][i][j].GetQIndex()));
				}
			}	

			//calculating distances bewteen elements and saving closest 3 distances and indices
			for(unsigned int i = 0; i < featureList1.size(); ++i){
				float dist1, dist2, dist3;
				int ind1, ind2, ind3;
				float dist = numeric_limits<float>::infinity();
				dist1 = dist; dist2 = dist; dist3 = dist;
				ind1=-1; ind2=-1; ind3=-1;
				for(unsigned int j = 0; j < featureList1.size(); ++j){
					if(i != j){
						dist = (featureList1[i].GetCentroid() - featureList1[j].GetCentroid()).length();
						if(dist < dist1){
							dist3 = dist2; ind3 = ind2;
							dist2 = dist1; ind2 = ind1;
							dist1 = dist; ind1 = j;
						}
						else if(dist < dist2){
							dist3 = dist2; ind3 = ind2;
							dist2 = dist; ind2 = j;
						}
						else if(dist < dist3){
							dist3 = dist; ind3 = j;
						}
					}
				}
				helixDistancesSorted.push_back(boost::tuples::tuple<int, int, int>(ind1, ind2, ind3));
			}			

		}



		void FlexibleFittingEngine::GetHelixEnds(int corrIx, int helixIx, Vec3F& end1, Vec3F& end2){
			end1 = featureList1[helixIx].GetEndPoint(0);
			end2 = featureList1[helixIx].GetEndPoint(1);
		}

		bool FlexibleFittingEngine::GetIsForward(int corrIx, int helixIx){
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						return corrs[corrIx][i][j].IsForward();
					}
				}
			}
			return false;
		}		

		//gets a transformation matrix to align the PDB rigidly to helixIx1 and helixIx2
		//used for rigid init
		MatrixFloat FlexibleFittingEngine::GetPairRigidTransform(int corrIx, int helixIx1, int helixIx2){
			if(helixIx1 == helixIx2){
				for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
					for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
						if(corrs[corrIx][i][j].GetPIndex() == helixIx1){
							cout << "returning cluster transform for cluster" << i << " with helixIx " << helixIx1 << endl;
							return GetRigidTransform2(corrIx, i);
						}
					}
				}
				
			}
			vector<SSECorrespondenceNode> tempCluster;
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					int p = corrs[corrIx][i][j].GetPIndex();
					if(p == helixIx1 || p == helixIx2){
						tempCluster.push_back(corrs[corrIx][i][j]);
					}
				}
			}

			return finder.GetTransform(tempCluster, SAMPLE_COUNT);
			
		}

		//runs the actual flexible deformation
		vector<Vec3F> FlexibleFittingEngine::Deform(int numNeighbors, MatrixFloat transform, bool rigidInitialization){
			if(transform.GetValue(3,3) > 0/5 && rigidInitialization){
				for(int i =0; i < origLocations.size(); ++i){
					origLocations[i] = origLocations[i].transform(transform);
				}
			}
			//if(rigidInitialization)
			//	return origLocations;
			//else
				return cd.Deform(origLocations, hardHandles, softHandles, numNeighbors);
		}

		//flattens a vector of Vec3F into a vector of floats where every 3 represent a point
		vector<float> FlexibleFittingEngine::returnFlattenedLocations(vector<Vec3F> in){
			vector<float> result;
			for(int i = 0; i < in.size(); ++i){
				result.push_back(in[i].X());
				result.push_back(in[i].Y());
				result.push_back(in[i].Z());
			}
			return result;
		}

		//adds an atom location used in the flexible deformation
		void FlexibleFittingEngine::addAtomLocation(Vec3F in){
			origLocations.push_back(in);
		}

		void FlexibleFittingEngine::addMeshVertex(float x, float y, float z) {
			mesh.AddVertex(Vec3F(x,y,z));
		}

		void FlexibleFittingEngine::addMeshEdge(int v1, int v2) {
			mesh.AddEdge(v1,v2);
		}

		void FlexibleFittingEngine::loadOffFile(string filename) {
			mesh.LoadOffFile(filename);
		}

		//adds a hard handle for the flexible deformation
		void FlexibleFittingEngine::addHardHandleLocation(Vec3F in){
			hardHandles.push_back(in);
		}

		void FlexibleFittingEngine::exportSkeletonAsMRC() {
			mesh.exportSkeletonToVolume();
		}

		//adds a soft handle for the flexible deformation
		void FlexibleFittingEngine::addSoftHandleLocation(Vec3F in){
			softHandles.push_back(in);
		}

		//resets the part of the engine needed for doing flexible fitting
		void FlexibleFittingEngine::prepareDeform(){
			hardHandles.clear();
			softHandles.clear();
			origLocations.clear();
		}


		//Sets weights for flexible deformation
		void FlexibleFittingEngine::setLaplacainW(float in){
			cd.setLaplacainW(in);
		}
		void FlexibleFittingEngine::setHardHandleW(float in){
			cd.setHardHandleW(in);
		}
		void FlexibleFittingEngine::setSoftHandleW(float in){
			cd.setSoftHandleW(in);
		}

		Vec3F FlexibleFittingEngine::getHelixCorner(int helixIx, int cornerIx) {
			return sse_renderer->getHelixCorner(helixIx, cornerIx);
		}

		//returns a vector arranged by QIndex as to whether or not that SSE element and its corresponding PDB are 'isforward' or not
		//according to the corrs data structure
		vector<bool> FlexibleFittingEngine::GetCorrespondenceFlips(int corrIx){
			vector<bool> result(featureList2.size());
			cout << "Size of result: " << result.size() << endl;
			cout << "Size of corrs: " << corrs[corrIx].size() << endl;
			for(unsigned int i = 0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); ++j){
					result[corrs[corrIx][i][j].GetQIndex()] = corrs[corrIx][i][j].IsForward();
				}
			}
			return result;
		}

		void FlexibleFittingEngine::addDeformationEdge(const int& firstVertexIndex, const int& secondVertexIndex) {
		    edges.push_back(make_pair(firstVertexIndex, secondVertexIndex));
		}

		void FlexibleFittingEngine::addCorrespondencesByReadIn(const int& pIndex, const int& qIndex, const bool& isForward) {

		    // When reading in the correspondence, we assume there is only one alignment and one correspondence in one cluster
		    if (corrs.empty())
		        corrs.push_back(vector< vector<SSECorrespondenceNode> >(1, vector<SSECorrespondenceNode>(1, SSECorrespondenceNode(pIndex, qIndex, isForward))));
		    else
		        corrs[0].push_back(vector<SSECorrespondenceNode>(1, SSECorrespondenceNode(pIndex, qIndex, isForward)));
		}

		void FlexibleFittingEngine::addDeformationOriginalVertex(const Vec3F& vertex) {
		    originalVertices.push_back(vertex);
		}


		//returns a vector containing only its
		//every pair of two numbers represents the first number (PDB index) and the second (SSE index)
		//are paired in corrs
		vector<int> FlexibleFittingEngine::GetAllCorrespondencesFlat(int corrIx){
			vector<int> result;
			for(unsigned int i = 0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); ++j){
					result.push_back(corrs[corrIx][i][j].GetPIndex());
					result.push_back(corrs[corrIx][i][j].GetQIndex());
				}
			}
			return result;
		}

		//returns the endpoints of the features in featueList1 as pairs of Vec3Fs
		vector<Vec3F> FlexibleFittingEngine::GetAllCAlphaFeatureVecsFlat(){
			vector<Vec3F> result;
			for(unsigned int i = 0; i < featureList1.size(); ++i){
				result.push_back(featureList1[i].GetEndPoint(0));
				result.push_back(featureList1[i].GetEndPoint(1));
			}			
			return result;
		}

		//Flips the orientation of a PDB SSE Match in corrs
		void FlexibleFittingEngine::FlipCorrespondencePair(int corrIx, int SSEIndex){
			for(unsigned int i=0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j=0; j < corrs[corrIx][i].size(); ++j){
					if(corrs[corrIx][i][j].GetQIndex() == SSEIndex){
						cout << "Flipping # " << j << " in " << i << "the cluster with QIndex " << corrs[corrIx][i][j].GetQIndex() << endl;
						cout << "Used to be: " << corrs[corrIx][i][j].IsForward() << endl;
						corrs[corrIx][i][j].SetForward(!corrs[corrIx][i][j].IsForward());
						cout << "Now is: " << corrs[corrIx][i][j].IsForward() << endl;
					}
				}
			}
		}

		SSECorrespondenceNode FlexibleFittingEngine::getCorrespondence(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex) {
		    return corrs[alignmentIndex][clusterIndex][featureIndex];
		}

		void FlexibleFittingEngine::computeCorrespondencePairWiseWeight(const int& selectedAlignmentIndex)
		{
			//std::cout << "Feature sizes:\n" << sourceFeatures.size() << ", " << targetFeatures.size() << "\n";
		    float sourceMinX =  MAX_DOUBLE, sourceMinY =  MAX_DOUBLE, sourceMinZ =  MAX_DOUBLE;
		    float sourceMaxX = -MAX_DOUBLE, sourceMaxY = -MAX_DOUBLE, sourceMaxZ = -MAX_DOUBLE;
		    float targetMinX =  MAX_DOUBLE, targetMinY =  MAX_DOUBLE, targetMinZ =  MAX_DOUBLE;
		    float targetMaxX = -MAX_DOUBLE, targetMaxY = -MAX_DOUBLE, targetMaxZ = -MAX_DOUBLE;

		    for (int clusterIndex = 0; clusterIndex < (int)corrs[selectedAlignmentIndex].size(); ++clusterIndex)
		        for (int featureIndex = 0; featureIndex < (int)corrs[selectedAlignmentIndex][clusterIndex].size(); ++featureIndex)
				{
		            // Cache the source feature, target feature and matching direction for each correspondence
		            SSECorrespondenceNode correspondence = getCorrespondence(selectedAlignmentIndex, clusterIndex, featureIndex);
		            int sourceIndex = correspondence.GetPIndex();
		            int targetIndex = correspondence.GetQIndex();
		            m_correspondenceSourceFeatureIndex.push_back(sourceIndex);
		            m_correspondenceTargetFeatureIndex.push_back(targetIndex);
		            m_correspondenceFlipFlag.push_back(correspondence.IsForward());
				}

		    for (auto it = sourceFeatures.begin(); it != sourceFeatures.end(); ++it)
		    {            
		        // Upate the AABB bounding box
		        sourceMinX = min(min(sourceMinX, it->GetEndPoint(0).X()), it->GetEndPoint(1).X());
		        sourceMinY = min(min(sourceMinY, it->GetEndPoint(0).Y()), it->GetEndPoint(1).Y());
		        sourceMinZ = min(min(sourceMinZ, it->GetEndPoint(0).Z()), it->GetEndPoint(1).Z());
		        sourceMaxX = max(max(sourceMaxX, it->GetEndPoint(0).X()), it->GetEndPoint(1).X());
		        sourceMaxY = max(max(sourceMaxY, it->GetEndPoint(0).Y()), it->GetEndPoint(1).Y());
		        sourceMaxZ = max(max(sourceMaxZ, it->GetEndPoint(0).Z()), it->GetEndPoint(1).Z());

		        //std::cout << "s\n";
		        //std::cout << std::setprecision(30) << it->GetEndPoint(0).X() << ", " << it->GetEndPoint(0).Y() << ", " << it->GetEndPoint(0).Z() << "\n";
		        //std::cout << std::setprecision(30) << it->GetEndPoint(1).X() << ", " << it->GetEndPoint(1).Y() << ", " << it->GetEndPoint(1).Z() << "\n";
		    }

		    for (auto it = targetFeatures.begin(); it != targetFeatures.end(); ++it)
		    {
		        targetMinX = min(min(targetMinX, it->GetEndPoint(0).X()), it->GetEndPoint(1).X());
		        targetMinY = min(min(targetMinY, it->GetEndPoint(0).Y()), it->GetEndPoint(1).Y());
		        targetMinZ = min(min(targetMinZ, it->GetEndPoint(0).Z()), it->GetEndPoint(1).Z());
		        targetMaxX = max(max(targetMaxX, it->GetEndPoint(0).X()), it->GetEndPoint(1).X());
		        targetMaxY = max(max(targetMaxY, it->GetEndPoint(0).Y()), it->GetEndPoint(1).Y());
		        targetMaxZ = max(max(targetMaxZ, it->GetEndPoint(0).Z()), it->GetEndPoint(1).Z());

		        //std::cout << "t\n";
		        //std::cout << std::setprecision(30) << it->GetEndPoint(0).X() << ", " << it->GetEndPoint(0).Y() << ", " << it->GetEndPoint(0).Z() << "\n";
		        //std::cout << std::setprecision(30) << it->GetEndPoint(1).X() << ", " << it->GetEndPoint(1).Y() << ", " << it->GetEndPoint(1).Z() << "\n";
		    }
		    
		    //double ratioBB = 0.3;
		    double sourceBBDiagonal = (Vec3D(sourceMaxX, sourceMaxY, sourceMaxZ) - Vec3D(sourceMinX, sourceMinY, sourceMinZ)).length();
		    double targetBBDiagonal = (Vec3D(targetMaxX, targetMaxY, targetMaxZ) - Vec3D(targetMinX, targetMinY, targetMinZ)).length();

		    // Gaussian weight --- 3*sigma == ratio*boundingBoxDiagonal
		    double sigma = m_bbRatio * min(sourceBBDiagonal, targetBBDiagonal) / 3.0;

			// For all the paired features, compute their weight based on midpoint distance
		    // Since we will fill in all the upper triangle, it is safe to use "std::vector<>().resize" here
		    m_correspondencePairWiseWeight.resize(m_correspondenceSourceFeatureIndex.size(), std::vector<double>(m_correspondenceSourceFeatureIndex.size()));
		    for (int i = 0; i < (int)m_correspondencePairWiseWeight.size(); ++i)
		        for (int j = i; j < (int)m_correspondencePairWiseWeight.size(); ++j)
		        {
		            if (i == j)
		            {
		                m_correspondencePairWiseWeight[i][j] = 1.0;
		                continue;
		            }

		            Vec3D sourceCentroidI = Vec3D(sourceFeatures[m_correspondenceSourceFeatureIndex[i]].GetCentroid().X(), sourceFeatures[m_correspondenceSourceFeatureIndex[i]].GetCentroid().Y(), sourceFeatures[m_correspondenceSourceFeatureIndex[i]].GetCentroid().Z());
		            Vec3D sourceCentroidJ = Vec3D(sourceFeatures[m_correspondenceSourceFeatureIndex[j]].GetCentroid().X(), sourceFeatures[m_correspondenceSourceFeatureIndex[j]].GetCentroid().Y(), sourceFeatures[m_correspondenceSourceFeatureIndex[j]].GetCentroid().Z());

		            Vec3D targetCentroidI = Vec3D(targetFeatures[m_correspondenceTargetFeatureIndex[i]].GetCentroid().X(), targetFeatures[m_correspondenceTargetFeatureIndex[i]].GetCentroid().Y(), targetFeatures[m_correspondenceTargetFeatureIndex[i]].GetCentroid().Z());
		            Vec3D targetCentroidJ = Vec3D(targetFeatures[m_correspondenceTargetFeatureIndex[j]].GetCentroid().X(), targetFeatures[m_correspondenceTargetFeatureIndex[j]].GetCentroid().Y(), targetFeatures[m_correspondenceTargetFeatureIndex[j]].GetCentroid().Z());

		            // sourceCenterDistance and targetCenterDistance
		            double scDist = (sourceCentroidI - sourceCentroidJ).length();
		            double tcDist = (targetCentroidI - targetCentroidJ).length();

		            m_correspondencePairWiseWeight[i][j] = std::exp(-0.5 * (scDist*scDist + tcDist*tcDist) / (sigma*sigma));
		        }

		    
		}

		//loads and saves 'corrs'.  Necessary because they get destroyed when a search is run and searchs have to be run
		void FlexibleFittingEngine::SaveCorrs(){
			savedCorrs = corrs;
		}
		void FlexibleFittingEngine::LoadSavedCorrs(){
			corrs = savedCorrs;
		}

}
}		