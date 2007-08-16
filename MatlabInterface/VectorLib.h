#ifndef VECTOR_LIB_H
#define VECTOR_LIB_H

#include "DataStructures.h"

namespace wustl_mm {
	namespace MatlabInterface {
		class VectorLib {
		public:
			static double DotProduct(Vector3D & v1, Vector3D & v2);
			static Vector3D CrossProduct(Vector3D & v1, Vector3D & v2);
			static Vector3D Difference(Vector3D & v1, Vector3D & v2);
			static Vector3D Addition(Vector3D & v1, Vector3D & v2);
			static Vector3D Initialize(double x, double y, double z);
			static Vector3DInt Initialize(int x, int y, int z);
			static void CrossProduct(Vector3D & vRet, Vector3D & v1, Vector3D & v2);
			static void Difference(Vector3D & vRet, Vector3D & v1, Vector3D & v2);
			static void Addition(Vector3D & vRet, Vector3D & v1, Vector3D & v2);
			static void Initialize(Vector3D & vRet, double x, double y, double z);
			static void Initialize(Vector3DInt & vRet, int x, int y, int z);
			static void Normalize(Vector3D & vRet);
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

		Vector3D VectorLib::Addition(Vector3D & v1, Vector3D & v2) {
			Vector3D vRet;
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

		void VectorLib::Addition(Vector3D & vRet, Vector3D & v1, Vector3D & v2) {
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



	}
}

#endif