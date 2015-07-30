#pragma once
#ifndef CURVEDEFORMER_H
#define CURVEDEFORMER_H

#include <vector>
#include <MathTools/Vector3D.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <tnt/tnt_sparse_matrix.h>
#include <tnt/tnt_matrix.h>
#include <tnt/tnt_linalg.h>
#include <Gorgon/CAlphaRenderer.h>
#include <Gorgon/MeshRenderer.h>
#include <Gorgon/SSERenderer.h>
#include "NonManifoldMesh.h"
#include <limits>

using namespace std;
using namespace wustl_mm::MathTools;
using namespace TNT;
using namespace TNT::Linear_Algebra;
using namespace wustl_mm::Protein_Morph;
using boost::numeric::ublas::matrix;
using boost::numeric::ublas::zero_matrix;
using boost::numeric::ublas::mapped_matrix;



class CurveDeformer
{
public:
	vector<Vector3DFloat> Deform(vector<Vector3DFloat>&, vector<Vector3DFloat>&, vector<Vector3DFloat>&, int);
	CurveDeformer(void);
	~CurveDeformer(void);
	void setLaplacainW(float);
	void setHardHandleW(float);
	void setSoftHandleW(float);

private:
	matrix<float> constructA(std::vector<Vector3DFloat>&, int, int);
	matrix<float> constructB(std::vector<Vector3DFloat>&, int);
	vector<Vector3DFloat> solveSystemQR(matrix<float>&, matrix<float>&, Matrix<float>&);
	vector<Vector3DFloat> solveSystemLU(matrix<float>&, matrix<float>&, Matrix<float>&);
	vector<Vector3DFloat> solveSystemCholesky(matrix<float>&, matrix<float>&, Matrix<float>&);
	matrix<float> constructAi(vector<Vector3DFloat> &, int, int);
	matrix<float> constructSi(vector<Vector3DFloat> &, int);
	vector<Vector3DFloat> calculateLaplacians(vector<Vector3DFloat>&);
	matrix<float> generateLaplacianCoefs(vector<Vector3DFloat>&, int);
	void addHandleConstraints(matrix<float> &, matrix<float> &, vector<Vector3DFloat>&, vector<Vector3DFloat>&);
	matrix<float> InvertMatrix (const matrix<float>& input);
	matrix<float> gjinverse(const boost::numeric::ublas::matrix<float> &m, bool &singular);


	float laplacianWeight;
	float hardHandleWeight;
	float softHandleWeight;
	vector<Vector3DFloat> startLocations;
	vector<int> helixStarts;
	vector<int> helixStops;
	vector<GeometricShape*> helices;
	CAlphaRenderer cAlphaRender;
	NonManifoldMesh_Annotated skeleton;
};




CurveDeformer::CurveDeformer(void)
{
	startLocations.clear();
	helixStarts.clear();
	helixStops.clear();
	laplacianWeight = 1;
	hardHandleWeight = 1;
	softHandleWeight = .05;
}


vector<Vector3DFloat> CurveDeformer::Deform(vector<Vector3DFloat>& originalLocations, vector<Vector3DFloat>& hardHandles, vector<Vector3DFloat>& softHandles, int numNeighbors)
{
	startLocations = originalLocations;

	int numHandles = 0;
	for(int i = 0; i < hardHandles.size(); ++i){
		if(hardHandles[i][0] != 0.0 && hardHandles[i][1] != 0.0 && hardHandles[i][2] != 0.0){
			++numHandles;
		}
	}
	for(int i = 0; i < softHandles.size(); ++i){
		if(softHandles[i][0] != 0.0 && softHandles[i][1] != 0.0 && softHandles[i][2] != 0.0){
			++numHandles;
		}
	}
	int extraConstraints = numHandles;

	//Getting the A and B Matrices
	matrix<float> A = constructA(originalLocations, extraConstraints, numNeighbors);
	matrix<float> B = constructB(originalLocations, extraConstraints);
	addHandleConstraints(A, B, hardHandles, softHandles);

	//Solving Ax=B for x
	//after deformation
	Matrix<float> flat;
	//vector<Vector3DFloat> resultCH = solveSystemCholesky(A,B, flat);
	vector<Vector3DFloat> resultLU = solveSystemLU(A,B, flat);
	//vector<Vector3DFloat> resultQR = solveSystemQR(A,B, flat);

	/*resid = transpose_mult(((A*flatOrig) - B),((A*flatOrig) - B));
	cout << "Residue from Original: " << resid(1,1) << endl;*/

	return resultLU;
}



matrix<float> CurveDeformer::constructA(vector<Vector3DFloat>& origPoints, int extraConstraints, int numNeighbors)
{
	int numPoints = origPoints.size();
	

	//3*(numPoints-2) rows because the endpoints dont have defineable laplacians
	mapped_matrix<float> result(3*(numPoints-2)+3*extraConstraints, 3*numPoints);
	vector<Vector3DFloat> laplacians = calculateLaplacians(origPoints);

	for(int i = 1; i < numPoints-1; ++i)
	{
		matrix<float> Ai = constructAi(origPoints, i, numNeighbors);
		matrix<float> Si = constructSi(laplacians, i);
		
		//Each point on the curve (except for ends) adds 3 constraints on to the A matrix
		//The three rows that get added to the matrix come from:
		//rows = Si*((Ai^t*Ai)^-1)*Ai^t
		//These terms have to be added to the correct location in the matrix
		bool hi =false;
		matrix<float> almostATerm = prod( Si,( gjinverse(prod(trans(Ai), Ai), hi)));
		if(hi == true)
			cout << "bad inverse" << endl;

		matrix<float> aTerm = prod(almostATerm, trans(Ai));	
		
		//this is the number of 0s to pad in front of Ai term
		int indent = max(0, 3*(i-numNeighbors));

		//insert Ai term by term into the matrix
		for(int j = 0; j < aTerm.size1(); ++j)		
		{
			int x = 3*(i-1)+j;
			for(int k = 0; k < aTerm.size2(); ++k)
			{				
				result(x, indent+k) = laplacianWeight*aTerm(j, k);
			}
		}
	}

	matrix<float> lapCoefs = generateLaplacianCoefs(origPoints, extraConstraints);
	matrix<float> A = laplacianWeight*(result - lapCoefs);
	return A;
}

matrix<float> CurveDeformer::constructB(vector<Vector3DFloat>& points, int extraConstraints)
{
	//B is all 0s except for handles
	mapped_matrix<float> result(3*(points.size()-2)+3*extraConstraints,1);
	return result;
}



matrix<float> CurveDeformer::generateLaplacianCoefs(vector<Vector3DFloat> & points, int numHandles)
{
	int numPoints = points.size();
	
	//mapped Matrix is sparse
	//3*(numPoints-2) rows because the endpoints dont have defineable laplacians
	mapped_matrix<float> result(3*(numPoints-2)+3*numHandles, 3*numPoints);

	//filling in the Matrix element by element
	for(int i = 1; i < numPoints -1; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			result(3*(i-1)+j, 3*(i-1)+j) = -0.5;
			result(3*(i-1)+j, 3*(i)+j) = 1;
			result(3*(i-1)+j, 3*(i+1)+j) = -0.5;			
		}
	}
	return result;
}


vector<Vector3DFloat> CurveDeformer::calculateLaplacians(vector<Vector3DFloat> & points)
{
	//storing them as Vector3D
	vector<Vector3DFloat> result(points.size());

	//laplacian is centroid of neighbors subtracted from original point
	for(int i = 1; i < points.size()-1; ++i)
	{
		result[i] = Vector3DFloat(points[i-1]*(-0.5) + points[i+1]*(-0.5) + points[i]);
	}
	return result;
}

matrix<float> CurveDeformer::constructAi(std::vector<Vector3DFloat> & points, int index, int numNeighbors)
{
	//minimum and maximum indictes of the neighbors to be used
	int mn = max(index-numNeighbors, 0);
	int mx = min((int)(index+numNeighbors), (int)points.size()-1);

	//size is 3*(total number of neighbors used) + 3 by 7
	matrix<float> result = matrix<double>(3*(mx-mn+1), 7);

	//format
	//x 0 z -y 1 0 0
	//y -z 0 x 0 1 0
	//z y -x 0 0 0 1
	//...
	//where x y and z are simply the 3d coordinates of that point

	for(int i = mn; i <= mx; ++i){
		//each block here is one row
		result(3*(i-mn), 0) = points[i][0];			result(3*(i-mn), 1) = 0;
		result(3*(i-mn), 2) = points[i][2];			result(3*(i-mn), 3) = -points[i][1];
		result(3*(i-mn), 4) = 1;					result(3*(i-mn), 5) = 0;					result(3*(i-mn), 6) = 0;

		result(3*(i-mn)+1, 0) = points[i][1];		result(3*(i-mn)+1, 1) = -points[i][2];
		result(3*(i-mn)+1, 2) = 0;					result(3*(i-mn)+1, 3) = points[i][0];
		result(3*(i-mn)+1, 4) = 0;					result(3*(i-mn)+1, 5) = 1;					result(3*(i-mn)+1, 6) = 0;

		result(3*(i-mn)+2, 0) = points[i][2];		result(3*(i-mn)+2, 1) = points[i][1];
		result(3*(i-mn)+2, 2) = -points[i][0];		result(3*(i-mn)+2, 3) = 0;
		result(3*(i-mn)+2, 4) = 0;					result(3*(i-mn)+2, 5) = 0;					result(3*(i-mn)+2, 6) = 1;
	}

	return result;

}

matrix<float> CurveDeformer::constructSi(std::vector<Vector3DFloat> & laplacians, int index)
{
	matrix<float> result = matrix<double>(3, 7);

	//format
	//x 0 z -y 0 0 0
	//y -z 0 x 0 0 0
	//z y -x 0 0 0 0

	//where x y and z are their respective components of the Laplacian vector for that point

	result(0, 0) = laplacians[index][0];	result(0, 1) = 0;
	result(0, 2) = laplacians[index][2];	result(0, 3) = -laplacians[index][1];
	result(0, 4) = 0;						result(0, 5) = 0;					result(0, 6) = 0;

	result(1, 0) = laplacians[index][1];	result(1, 1) = -laplacians[index][2];
	result(1, 2) = 0;						result(1, 3) = laplacians[index][0];
	result(1, 4) = 0;						result(1, 5) = 0;					result(1, 6) = 0;

	result(2, 0) = laplacians[index][2];	result(2, 1) = laplacians[index][1];
	result(2, 2) = -laplacians[index][0];	result(2, 3) = 0;
	result(2, 4) = 0;						result(2, 5) = 0;					result(2, 6) = 0;

	return result;

}

void CurveDeformer::addHandleConstraints(matrix<float> & A, matrix<float> & B, vector<Vector3DFloat>& hardHandles, vector<Vector3DFloat>& softHandles)
{
	int totalNumPoints = hardHandles.size();
	int minRow = 3 * (totalNumPoints-2);

	//hard handles are the helices, all three entries are 0.0 when that vertex is not part of a helix
	//i.e. (0,0,0) if you dont want a handle on that point
	for(int i = 0; i < totalNumPoints; ++i){
		if(hardHandles[i][0] != 0.0 && hardHandles[i][1] != 0.0 && hardHandles[i][2] != 0.0){
				A(minRow, 3*i) = hardHandleWeight*1;
				A(minRow+1, 3*i+1) = hardHandleWeight*1;
				A(minRow+2, 3*i+2) = hardHandleWeight*1;

				B(minRow, 0) = hardHandleWeight*hardHandles[i][0];
				B(minRow+1, 0) = hardHandleWeight*hardHandles[i][1];
				B(minRow+2, 0) = hardHandleWeight*hardHandles[i][2];

				minRow = minRow+3;
		}
	}


	//soft handles are everything else, they are given a smaller weight
	for(int i = 0; i < totalNumPoints; ++i){		
		if(softHandles[i][0] != 0.0 && softHandles[i][1] != 0.0 && softHandles[i][2] != 0.0){
				A(minRow, 3*i) = softHandleWeight*1;
				A(minRow+1, 3*i+1) = softHandleWeight*1;
				A(minRow+2, 3*i+2) = softHandleWeight*1;

				B(minRow, 0) = softHandleWeight*softHandles[i][0];
				B(minRow+1, 0) = softHandleWeight*softHandles[i][1];
				B(minRow+2, 0) = softHandleWeight*softHandles[i][2];

				minRow = minRow+3;
		}
	}

}




void CurveDeformer::setLaplacainW(float in){
	laplacianWeight = in;
}
void CurveDeformer::setHardHandleW(float in){
	hardHandleWeight = in;
}
void CurveDeformer::setSoftHandleW(float in){
	softHandleWeight = in;
}


//------------------------------Linear System Solvers------------------------------------//
vector<Vector3DFloat> CurveDeformer::solveSystemQR(matrix<float>& A, matrix<float>& B, Matrix<float>& flat)
{
	bool singular = false;
	matrix<float> AABoost = prod(trans(A),A);
	matrix<float> ABBoost = prod(trans(A),B);
	Matrix<float> AATNT(AABoost.size1(), AABoost.size2());
	Matrix<float> ABTNT(ABBoost.size1(), ABBoost.size2());
	
	for(int i = 0; i < AABoost.size1(); ++i){
		for(int j = 0; j < AABoost.size2(); ++j){
			AATNT(i+1, j+1) = AABoost(i,j);}}

	for(int i = 0; i < ABBoost.size1(); ++i){
		for(int j = 0; j < ABBoost.size2(); ++j){
			ABTNT(i+1, j+1) = ABBoost(i,j);}}


	QR<float> qr(AATNT);
	Matrix<float> solved = qr.solve(ABTNT);
	flat = solved;
	vector<Vector3DFloat> result(solved.num_rows()/3);

	//fill the transformed vertices back into a vector
	for(int i = 0; i < solved.num_rows(); i = i+3)
	{
		result[i/3] = Vector3DFloat(solved(i+1,1), solved(i+2,1), solved(i+3,1));
	}

	return result;

}

vector<Vector3DFloat> CurveDeformer::solveSystemLU(matrix<float>& A, matrix<float>& B, Matrix<float>& flat)
{
	bool singular = false;

	//cout << "started two mults" << endl;
	matrix<float> AABoost = prod(trans(A),A);
	matrix<float> ABBoost = prod(trans(A),B);
	Matrix<float> AATNT(AABoost.size1(), AABoost.size2());
	Matrix<float> ABTNT(ABBoost.size1(), ABBoost.size2());

	//cout << "ended mults, started copy" << endl;
	for(int i = 0; i < AABoost.size1(); ++i){
		for(int j = 0; j < AABoost.size2(); ++j){
			AATNT(i+1, j+1) = AABoost(i,j);}}

	for(int i = 0; i < ABBoost.size1(); ++i){
		for(int j = 0; j < ABBoost.size2(); ++j){
			ABTNT(i+1, j+1) = ABBoost(i,j);}}

	//cout << "ended copy, started solve" << endl;

	LU<float> lu(AATNT);
	Matrix<float> solved = lu.solve(ABTNT);
	flat = solved;
	vector<Vector3DFloat> result(solved.num_rows()/3);

	//fill the transformed vertices back into a vector
	for(int i = 0; i < solved.num_rows(); i = i+3)
	{
		result[i/3] = Vector3DFloat(solved(i+1,1), solved(i+2,1), solved(i+3,1));
	}

	return result;

}

vector<Vector3DFloat> CurveDeformer::solveSystemCholesky(matrix<float>& A, matrix<float>& B, Matrix<float>& flat)
{
	bool singular = false;
	//cout << "started two mults" << endl;
	matrix<float> AABoost = prod(trans(A),A);
	matrix<float> ABBoost = prod(trans(A),B);
	Matrix<float> AATNT(AABoost.size1(), AABoost.size2());
	Matrix<float> ABTNT(ABBoost.size1(), ABBoost.size2());
	
	for(int i = 0; i < AABoost.size1(); ++i){
		for(int j = 0; j < AABoost.size2(); ++j){
			AATNT(i+1, j+1) = AABoost(i,j);}}

	for(int i = 0; i < ABBoost.size1(); ++i){
		for(int j = 0; j < ABBoost.size2(); ++j){
			ABTNT(i+1, j+1) = ABBoost(i,j);}}
	//cout << "ended mults, started solve" << endl;

	Cholesky<float> ch(AATNT);
	Matrix<float> solved = ch.solve(ABTNT);
	flat = solved;
	vector<Vector3DFloat> result(solved.num_rows()/3);

	//fill the transformed vertices back into a vector
	for(int i = 0; i < solved.num_rows(); i = i+3)
	{
		result[i/3] = Vector3DFloat(solved(i+1,1), solved(i+2,1), solved(i+3,1));
	}

	return result;

}


//--------------------------------Matrix Inverse Code---------------------------------//
//Matrix inversion code modified from:
//http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion
matrix<float> CurveDeformer::InvertMatrix (const matrix<float>& input) {
	
	//for(int i = 0 ; i < input.size1(); ++i)
	//{
	//	for(int j = 0; j < input.size2(); ++j)
	//	{
	//		cout << input(i,j) << " ";
	//	}
	//	cout << "\n";
	//}
	//cout << "\n";

	matrix<float> inverse(input.size1(), input.size2());
	namespace ublas = boost::numeric::ublas;
	using namespace boost::numeric::ublas;
	typedef permutation_matrix<std::size_t> pmatrix;
	// create a working copy of the input
	matrix<float> A(input);
	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());
	// perform LU-factorization
	int res = lu_factorize(A,pm);
	
	//Error state
	if( res != 0 ) 
		return inverse;
	// create identity matrix of "inverse"
	inverse.assign(ublas::identity_matrix<float>(A.size1()));
	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);
	return inverse;
}

boost::numeric::ublas::matrix<float> CurveDeformer::gjinverse(const boost::numeric::ublas::matrix<float> &m, bool &singular)
 {
     using namespace boost::numeric::ublas;
     const int size = m.size1();
     // Cannot invert if non-square matrix or 0x0 matrix.
     // Report it as singular in these cases, and return 
     // a 0x0 matrix.
     if (size != m.size2() || size == 0)
     {
         singular = true;
         matrix<float> A(0,0);
         return A;
     }
     // Handle 1x1 matrix edge case as general purpose 
     // inverter below requires 2x2 to function properly.
     if (size == 1)
     {
         matrix<float> A(1, 1);
         if (m(0,0) == 0.0)
         {
             singular = true;
             return A;
         }
         singular = false;
         A(0,0) = 1/m(0,0);
         return A;
     }
     // Create an augmented matrix A to invert. Assign the
     // matrix to be inverted to the left hand side and an
     // identity matrix to the right hand side.
     matrix<float> A(size, 2*size);
     matrix_range<matrix<float> > Aleft(A, 
                                    range(0, size), 
                                    range(0, size));
     Aleft = m;
     matrix_range<matrix<float> > Aright(A, 
                                     range(0, size), 
                                     range(size, 2*size));
     Aright = identity_matrix<float>(size);

     // Doing partial pivot
     for (int k = 0; k < size; k++)
     {
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 // RIGHT PLACEMENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     // Swap rows to eliminate zero diagonal elements.
     for (int k = 0; k < size; k++)
     {
         if ( A(k,k) == 0 ) // XXX: test for "small" instead
         {
             // Find a row(l) to swap with row(k)
             int l = -1;
             for (int i = k+1; i < size; i++) 
             {
                 if ( A(i,k) != 0 )
                 {
                     l = i; 
                     break;
                 }
             }
             // Swap the rows if found
             if ( l < 0 ) 
             {
                 std::cerr << "Error:" <<  __FUNCTION__ << ":"
                           << "Input matrix is singular, because cannot find"
                           << " a row to swap while eliminating zero-diagonal.";
                 singular = true;
                 return Aleft;
             }
             else 
             {
                 matrix_row<matrix<float> > rowk(A, k);
                 matrix_row<matrix<float> > rowl(A, l);
                 rowk.swap(rowl);

             }
         }
	 }
 ///////////////////////////////////////////////////////////////////////////////////////////////////////// 
         // normalize the current row
         for (int j = k+1; j < 2*size; j++)
             A(k,j) /= A(k,k);
         A(k,k) = 1;
         // normalize other rows
         for (int i = 0; i < size; i++)
         {
             if ( i != k )  // other rows  // FIX: PROBLEM HERE
             {
                 if ( A(i,k) != 0 )
                 {
                     for (int j = k+1; j < 2*size; j++)
                         A(i,j) -= A(k,j) * A(i,k);
                     A(i,k) = 0;
                 }
             }
         }

     }
     singular = false;
     return Aright;
 }





CurveDeformer::~CurveDeformer(void)
{
}

#endif