#ifndef VECTOR3D_H
#define VECTOR3D_H

namespace wustl_mm {
	namespace MatlabInterface {
		class MyVector3D {
		public:
			MyVector3D();
			MyVector3D(double x, double y, double z);
			~MyVector3D();

			double X();
			double Y();
			double Z();

			double MyVector3D::operator*(MyVector3D &d ); // Dot Product

			MyVector3D operator+(MyVector3D &d );
			MyVector3D operator-();
			MyVector3D operator-(MyVector3D &d );
			MyVector3D operator^(MyVector3D &d );		 // Cross Product
			MyVector3D operator*(double s);			
			MyVector3D& operator+=(const MyVector3D& d);
			MyVector3D& operator-=(const MyVector3D& d);
			MyVector3D Rotate(MyVector3D axis, double angle);

			static void Normalize(MyVector3D d);

			double values[3];
		};

		MyVector3D::MyVector3D() {
			values[0] = 0.0;
			values[1] = 0.0;
			values[2] = 0.0;
		}

		MyVector3D::MyVector3D(double x, double y, double z) {
			values[0] = x;
			values[1] = y;
			values[2] = z;
		}

		MyVector3D::~MyVector3D() {
		}

		double MyVector3D::X() {
			return values[0];
		}

		double MyVector3D::Y() {
			return values[1];
		}

		double MyVector3D::Z() {
			return values[2];
		}


		double MyVector3D::operator*(MyVector3D &d ) {		// Dot Product
			return X() * d.X() + Y() * d.Y() + Z() * d.Z();
		}

		MyVector3D MyVector3D::operator+(MyVector3D &d ) {
			return MyVector3D(X() + d.X(), Y() + d.Y(), Z() + d.Z());	
		}

		MyVector3D MyVector3D::operator-() {
			return MyVector3D(-X(), -Y(), -Z());	
		}

		MyVector3D MyVector3D::operator-(MyVector3D &d ) {
			return MyVector3D(X() - d.X(), Y() - d.Y(), Z() - d.Z());	
		}

		MyVector3D MyVector3D::operator^(MyVector3D &d ) { // Cross Product
			return MyVector3D(
				values[1] * d.values[2] - values[2] * d.values[1], 
				values[2] * d.values[0] - values[0] * d.values[2], 
				values[0] * d.values[1] - values[1] * d.values[0]);	
		}

		MyVector3D MyVector3D::operator*(double s) {
			return MyVector3D(X()*s, Y()*s, Z()*s);
		}

		MyVector3D& MyVector3D::operator+=(const MyVector3D& d) {
			for(int i = 0; i < 3; i++) {
				values[i] += d.values[i];
			}
			return *this;
		}

		MyVector3D& MyVector3D::operator-=(const MyVector3D& d) {
			for(int i = 0; i < 3; i++) {
				values[i] -= d.values[i];
			}
			return *this;
		}
		MyVector3D Normalize(MyVector3D d) {
			double base = 0;
			for(int i = 0; i < 3; i++) {
				base += d.values[i] * d.values[i];
			}
			if(base == 0) {
				return MyVector3D();
			} else {				
				base = sqrt(base);
				return MyVector3D(d.X() / base, d.Y() / base, d.Z() / base); 
			}
		}

		MyVector3D MyVector3D::Rotate(MyVector3D axis, double angle) {
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


			MyVector3D v = MyVector3D(0.0, 0.0, 0.0);
			for(int x = 0; x < 3; x++) {
				for(int y = 0; y < 3; y++) {
					v.values[x] = v.values[x] + R[y][x] * values[y];
				}
			}
			return v;
		}
	}
}

#endif