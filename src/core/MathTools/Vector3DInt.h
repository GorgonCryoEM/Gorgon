/*
#ifndef VECTOR3D_INT_H
#define VECTOR3D_INT_H

namespace wustl_mm {
	namespace MathTools {
		
		class Vector3DInt {
		public:
			Vector3DInt();
			Vector3DInt(int x, int y, int z);
			~Vector3DInt();

			int X();
			int Y();
			int Z();

			bool operator!=(Vector3DInt &d);
			bool operator==(Vector3DInt &d);						
			Vector3DInt operator+(Vector3DInt &d );
			Vector3DInt operator-();
			Vector3DInt operator-(Vector3DInt &d );
			Vector3DInt operator*(int s);			
			Vector3DInt& operator=(const Vector3DInt& d);
			Vector3DInt& operator+=(const Vector3DInt& d);
			Vector3DInt& operator-=(const Vector3DInt& d);
		public:
			int values[3];
		};

		Vector3DInt::Vector3DInt() {
			values[0] = 0;
			values[1] = 0;
			values[2] = 0;
		}

		Vector3DInt::Vector3DInt(int x, int y, int z) {
			values[0] = x;
			values[1] = y;
			values[2] = z;
		}

		Vector3DInt::~Vector3DInt() {
		}

		int Vector3DInt::X() {
			return values[0];
		}

		int Vector3DInt::Y() {
			return values[1];
		}

		int Vector3DInt::Z() {
			return values[2];
		}



		bool Vector3DInt::operator!=(Vector3DInt &d) {
			return (X() != d.X()) || (Y() != d.Y()) || (Z() != d.Z());
		}

		bool Vector3DInt::operator==(Vector3DInt &d) {
			return (X() == d.X()) && (Y() == d.Y()) && (Z() == d.Z());
		}

		Vector3DInt Vector3DInt::operator+(Vector3DInt &d ) {
			return Vector3DInt(X() + d.X(), Y() + d.Y(), Z() + d.Z());	
		}

		Vector3DInt Vector3DInt::operator-() {
			return Vector3DInt(-X(), -Y(), -Z());	
		}

		Vector3DInt Vector3DInt::operator-(Vector3DInt &d ) {
			return Vector3DInt(X() - d.X(), Y() - d.Y(), Z() - d.Z());	
		}

		Vector3DInt Vector3DInt::operator*(int s) {
			return Vector3DInt(X()*s, Y()*s, Z()*s);
		}

		Vector3DInt& Vector3DInt::operator+=(const Vector3DInt& d) {
			for(int i = 0; i < 3; i++) {
				values[i] += d.values[i];
			}
			return *this;
		}

		Vector3DInt& Vector3DInt::operator-=(const Vector3DInt& d) {
			for(int i = 0; i < 3; i++) {
				values[i] -= d.values[i];
			}
			return *this;
		}
		Vector3DInt& Vector3DInt::operator=(const Vector3DInt& d) {
			for(int i = 0; i < 3; i++) {
				values[i] = d.values[i];
			}
			return *this;
		}
	}
}

#endif
*/