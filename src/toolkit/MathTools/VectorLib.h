#ifndef VECTOR_LIB_H
#define VECTOR_LIB_H

#include "DataStructures.h"

namespace wustl_mm {
	namespace MathTools {
		class VectorLib {
		public:
			static double DotProduct(Vector3D & v1, Vector3D & v2);
			static Vector3D CrossProduct(Vector3D & v1, Vector3D & v2);
			static Vector3D Difference(Vector3D & v1, Vector3D & v2);
			static Vector3DInt Difference(Vector3DInt & v1, Vector3DInt & v2);
			static Vector3D Addition(Vector3D & v1, Vector3D & v2);
			static Vector3D ScalarMultiply(double s, Vector3D & v);
			static Vector3DInt Addition(Vector3DInt & v1, Vector3DInt & v2);
			static Vector3D Initialize(double x, double y, double z);
			static Vector3DInt Initialize(int x, int y, int z);
			static void CrossProduct(Vector3D & vRet, Vector3D & v1, Vector3D & v2);
			static void Difference(Vector3D & vRet, Vector3D & v1, Vector3D & v2);
			static void Difference(Vector3DInt & vRet, Vector3DInt & v1, Vector3DInt & v2);
			static void Addition(Vector3D & vRet, Vector3D & v1, Vector3D & v2);
			static void Addition(Vector3DInt & vRet, Vector3DInt & v1, Vector3DInt & v2);
			static void Initialize(Vector3D & vRet, double x, double y, double z);
			static void Initialize(Vector3DInt & vRet, int x, int y, int z);
			static void Normalize(Vector3D & vRet);
			static Quarternion VectorsToQuarternion(Vector3D v1, Vector3D v2, Vector3D v3);
			static Vector3D Rotate(Vector3D vector, Vector3D axis, double angle);
		};

		double VectorLib::DotProduct(Vector3D & v1, Vector3D & v2) {
			return v1.values[0] * v2.values[0] + v1.values[1] * v2.values[1] + v1.values[2] * v2.values[2];
		}

		Vector3D VectorLib::CrossProduct(Vector3D & v1, Vector3D & v2) {
			Vector3D vRet;
			CrossProduct(vRet, v1, v2);
			return vRet;
		}

		Vector3D VectorLib::Difference(Vector3D & v1, Vector3D & v2) {
			Vector3D vRet;
			Difference(vRet, v1, v2);
			return vRet;
		}	

		Vector3DInt VectorLib::Difference(Vector3DInt & v1, Vector3DInt & v2) {
			Vector3DInt vRet;
			Difference(vRet, v1, v2);
			return vRet;
		}	

		Vector3D VectorLib::Addition(Vector3D & v1, Vector3D & v2) {
			Vector3D vRet;
			Addition(vRet, v1, v2);
			return vRet;
		}

		Vector3D VectorLib::ScalarMultiply(double s, Vector3D & v) {
			return Initialize(s*v.values[0], s*v.values[1], s*v.values[2]);
		}

		Vector3DInt VectorLib::Addition(Vector3DInt & v1, Vector3DInt & v2) {
			Vector3DInt vRet;
			Addition(vRet, v1, v2);
			return vRet;
		}

		Vector3D VectorLib::Initialize(double x, double y, double z) {
			Vector3D vRet;
			Initialize(vRet, x, y, z);
			return vRet;
		}

		Vector3DInt VectorLib::Initialize(int x, int y, int z) {
			Vector3DInt vRet;
			Initialize(vRet, x, y, z);
			return vRet;
		}

		void VectorLib::CrossProduct(Vector3D & vRet, Vector3D & v1, Vector3D & v2) {
			vRet.values[0] = v1.values[1] * v2.values[2] - v1.values[2] * v2.values[1];
			vRet.values[1] = v1.values[2] * v2.values[0] - v1.values[0] * v2.values[2];
			vRet.values[2] = v1.values[0] * v2.values[1] - v1.values[1] * v2.values[0];
		}


		void VectorLib::Difference(Vector3D & vRet, Vector3D & v1, Vector3D & v2) {
			for(int i = 0; i < 3; i++) {
				vRet.values[i] = v1.values[i] - v2.values[i];
			}
		}

		void VectorLib::Difference(Vector3DInt & vRet, Vector3DInt & v1, Vector3DInt & v2) {
			for(int i = 0; i < 3; i++) {
				vRet.values[i] = v1.values[i] - v2.values[i];
			}
		}


		void VectorLib::Addition(Vector3D & vRet, Vector3D & v1, Vector3D & v2) {
			for(int i = 0; i < 3; i++) {
				vRet.values[i] = v1.values[i] + v2.values[i];
			}
		}

		void VectorLib::Addition(Vector3DInt & vRet, Vector3DInt & v1, Vector3DInt & v2) {
			for(int i = 0; i < 3; i++) {
				vRet.values[i] = v1.values[i] + v2.values[i];
			}
		}

		void VectorLib::Initialize(Vector3D & v, double x, double y, double z) {
			v.values[0] = x;
			v.values[1] = y;
			v.values[2] = z;
		}

		void VectorLib::Initialize(Vector3DInt & v, int x, int y, int z) {
			v.values[0] = x;
			v.values[1] = y;
			v.values[2] = z;
		}

		void VectorLib::Normalize(Vector3D & vRet) {
			double base = 0;
			for(int i = 0; i < 3; i++) {
				base += vRet.values[i] * vRet.values[i];
			}
			if(base == 0) {
				for(int i = 0; i < 3; i++) {
					vRet.values[i] = 0;
				}
			} else {
				base = sqrt(base);
				for(int i = 0; i < 3; i++) {
					vRet.values[i] = vRet.values[i] / base;
				}
			}
		}



		Quarternion VectorLib::VectorsToQuarternion(Vector3D v1, Vector3D v2, Vector3D v3) {
			//Quarternion quat;	
			//double m[3][3] = {{v1.values[0], v1.values[1], v1.values[2]},	// Correct I think
			//				  {v2.values[0], v2.values[1], v2.values[2]},
			//				  {v3.values[0], v3.values[1], v3.values[2]}};

			////double m[3][3] = {{v1.values[0], v2.values[0], v3.values[0]},   	
			////				  {v1.values[1], v2.values[1], v3.values[1]},
			////				  {v1.values[2], v2.values[2], v3.values[2]}};

			//double tr, s, q[4];
			//int i, j, k;
			//int nxt[3] = {1, 2, 0};
			//tr = m[0][0] + m[1][1] + m[2][2];
			//// check the diagonal
			//if (tr > 0.0) {
			//	s = sqrt (tr + 1.0);
			//	quat.values[3] = s / 2.0;
			//	s = 0.5 / s;
			//	quat.values[0] = (m[1][2] - m[2][1]) * s;
			//	quat.values[1] = (m[2][0] - m[0][2]) * s;
			//	quat.values[2] = (m[0][1] - m[1][0]) * s;
			//} else {		
			//	i = 0;
			//	if (m[1][1] > m[0][0]) {
			//		i = 1;
			//	}
			//	if (m[2][2] > m[i][i]) {
			//		i = 2;
			//	}
			//	j = nxt[i];
			//	k = nxt[j];
			//	s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
			//	q[i] = s * 0.5;

			//	if (s != 0.0) {
			//		s = 0.5 / s;
			//	}

			//	q[3] = (m[j][k] - m[k][j]) * s;
			//	q[j] = (m[i][j] + m[j][i]) * s;
			//	q[k] = (m[i][k] + m[k][i]) * s;
	
			//	quat.values[0] = q[0];
			//	quat.values[1] = q[1];
			//	quat.values[2] = q[2];
			//	quat.values[3] = q[3];
			//}
			//return quat;		


			Quarternion q;


			//double a[3][3] = {{v1.values[0], v1.values[1], v1.values[2]},	
			//				  {v2.values[0], v2.values[1], v2.values[2]},
			//				  {v3.values[0], v3.values[1], v3.values[2]}};

			double a[3][3] = {{v1.values[0], v2.values[0], v3.values[0]}, // Correct I think
							  {v1.values[1], v2.values[1], v3.values[1]},
							  {v1.values[2], v2.values[2], v3.values[2]}};

			double trace = a[0][0] + a[1][1] + a[2][2] + 1.0;
			if( trace > 0.000001 ) {
				double s = 0.5 / sqrt(trace);
				q.values[3] = 0.25 / s;
				q.values[0] = ( a[2][1] - a[1][2] ) * s;
				q.values[1] = ( a[0][2] - a[2][0] ) * s;
				q.values[2] = ( a[1][0] - a[0][1] ) * s;
			} else {
				if ( a[0][0] > a[1][1] && a[0][0] > a[2][2] ) {
					double s = 2.0 * sqrt( 1.0 + a[0][0] - a[1][1] - a[2][2]);
					q.values[3] = (a[1][2] - a[2][1] ) / s;
					q.values[0] = 0.25 * s;
					q.values[1] = (a[0][1] + a[1][0] ) / s;
					q.values[2] = (a[0][2] + a[2][0] ) / s;
				} else if (a[1][1] > a[2][2]) {
					double s = 2.0 * sqrt( 1.0 + a[1][1] - a[0][0] - a[2][2]);
					q.values[3] = (a[0][2] - a[2][0] ) / s;
					q.values[0] = (a[0][1] + a[1][0] ) / s;
					q.values[1] = 0.25 * s;
					q.values[2] = (a[1][2] + a[2][1] ) / s;
				} else {
					double s = 2.0 * sqrt( 1.0 + a[2][2] - a[0][0] - a[1][1] );
					q.values[3] = (a[0][1] - a[1][0] ) / s;
					q.values[0] = (a[0][2] + a[2][0] ) / s;
					q.values[1] = (a[1][2] + a[2][1] ) / s;
					q.values[2] = 0.25 * s;
				}
			}
			return q;
		}
		Vector3D VectorLib::Rotate(Vector3D vector, Vector3D axis, double angle) {
			double r = angle;
			double a = axis.values[0];
			double b = axis.values[1];
			double c = axis.values[2];
			
			double q0 = cos(r/2.0);
			double q1 = sin(r/2.0)* a;
			double q2 = sin(r/2.0)* b;
			double q3 = sin(r/2.0)* c;


			double R[3][3] = {{q0*q0 + q1*q1 - q2*q2 - q3*q3,		2*(q1*q2 - q0*q3),					2*(q1*q3 + q0*q2)},
							  {2*(q2*q1 + q0*q3),					q0*q0 - q1*q1 + q2*q2 - q3*q3,		2*(q2*q3 - q0*q1)},
							  {2*(q3*q1 - q0*q2),					2*(q3*q2 + q0*q1),					q0*q0 - q1*q1 - q2*q2 + q3*q3}};



			//double c = cos (angle); 
			//double s = sin (angle);
			//double t = 1-cos (angle);
			//double X = axis.values[0];
			//double Y = axis.values[1];
			//double Z = axis.values[2];

			//double R[3][3] = {{t*X*X + c, t*X*Y - s*Z, t*X*Z + s*Y},
			//				  {t*X*Y + s*Z, t*Y*Y + c, t*Y*Z - s*X},
			//				  {t*X*Z - s*Y, t*Y*Z + s*X, t*Z*Z + c}};

			Vector3D v = VectorLib::Initialize(0.0,0.0,0.0);
			for(int x = 0; x < 3; x++) {
				for(int y = 0; y < 3; y++) {
					v.values[x] = v.values[x] + R[y][x] * vector.values[y];
				}
			}
			return v;
		}
	}
}

#endif