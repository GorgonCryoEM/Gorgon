// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A simulated annealing based fitting tool to fit a secondary structure element into the density.


#include <SkeletonMaker/volume.h>
#include <MathTools/Vector3D.h>
#include <GraphMatch/GeometricShape.h>

#ifndef PROTEINMORPH_SSE_FLEXIBLE_FITTER
#define PROTEINMORPH_SSE_FLEXIBLE_FITTER

using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::Visualization;
using namespace wustl_mm::SkeletonMaker;


namespace wustl_mm {
	namespace Protein_Morph {

		struct SheetIdsAndSelect {
			unsigned char id;
			bool selected;
		};

		typedef NonManifoldMesh<bool, bool, SheetIdsAndSelect> NonManifoldMesh_SheetIds;

		class SSEFlexibleFitter{
		public:
			SSEFlexibleFitter(Volume * vol);
			~SSEFlexibleFitter();
			void FitHelix(GeometricShape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount);
			void FitSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount);

		private:
			Vector3DFloat GetForceOnPoint(Vector3DFloat pt);
			Vector3DFloat GetForceOnHelix(Vector3DFloat p, Vector3DFloat q, double discretizationStepSize);
			Vector3DFloat GetForceOnSheet(vector<Vector3DFloat> pts);
			Vector3DFloat GetTorqueOnPoint(Vector3DFloat pt, Vector3DFloat center);
			Vector3DFloat GetTorqueOnHelix(Vector3DFloat p, Vector3DFloat q, double discretizationStepSize);
			Vector3DFloat GetTorqueOnSheet(vector<Vector3DFloat> pts);
			Vector3DFloat WorldPointToVolume(Vector3DFloat pt);
			Vector3DFloat VolumePointToWorld(Vector3DFloat pt);
			Vector3DFloat VolumeVectorToWorld(Vector3DFloat pt);
			void MoveHelix(GeometricShape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize);
			void MoveSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize);

		private:
			Vector3DFloat * forceField;
			Volume * volume;

		};

		SSEFlexibleFitter::SSEFlexibleFitter(Volume * vol) {
			this->volume = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol);
			this->volume->normalize(0.0, 1.0);
			int sizex, sizey, sizez, ix;
			sizex = volume->getSizeX();
			sizey = volume->getSizeY();
			sizez = volume->getSizeZ();

			forceField = new Vector3DFloat[sizex * sizey * sizez];
			for(int x = 0; x < sizex; x++) {
				for(int y = 0; y < sizey; y++) {
					for(int z = 0; z < sizez; z++) {
						ix = volume->getIndex(x, y, z);
						forceField[ix] = Vector3DFloat(0, 0, 0);

						if((x != 0) && (y != 0) && (z != 0) && (x != sizex-1) && (y != sizey-1) && (z != sizez-1)) {						

							for(int xx = -1; xx <= 1; xx++) {
								for(int yy = -1; yy <= 1; yy++) {
									for(int zz = -1; zz <= 1; zz++) {
										forceField[ix] += Vector3DFloat::Normalize(Vector3DFloat(xx, yy, zz)) * (volume->getDataAt(x+xx, y+yy, z+zz) - volume->getDataAt(ix));
									}
								}
							}
						}
						
					}
				}
			}

		}

		SSEFlexibleFitter::~SSEFlexibleFitter() {
			delete [] forceField;
			delete volume;
		}

		Vector3DFloat SSEFlexibleFitter::WorldPointToVolume(Vector3DFloat pt) {
			return Vector3DFloat(
				(pt.X() - volume->getOriginX()) / volume->getSpacingX(), 
				(pt.Y() - volume->getOriginY()) / volume->getSpacingY(), 
				(pt.Z() - volume->getOriginZ()) / volume->getSpacingZ());
		}

		Vector3DFloat SSEFlexibleFitter::VolumePointToWorld(Vector3DFloat pt) {
			return Vector3DFloat (
				volume->getOriginX() + pt.X() * volume->getSpacingX(),
				volume->getOriginY() + pt.Y() * volume->getSpacingY(),
				volume->getOriginZ() + pt.Z() * volume->getSpacingZ());
		}

		Vector3DFloat SSEFlexibleFitter::VolumeVectorToWorld(Vector3DFloat pt) {
			return Vector3DFloat (
				pt.X() * volume->getSpacingX(),
				pt.Y() * volume->getSpacingY(),
				pt.Z() * volume->getSpacingZ());
		}

		Vector3DFloat SSEFlexibleFitter::GetForceOnPoint(Vector3DFloat pt) {
			int baseX = (int)floorf(pt.X());
			int baseY = (int)floorf(pt.Y());
			int baseZ = (int)floorf(pt.Z());
			if((baseX >= 0) && (baseY >= 0) && (baseZ >= 0) && (baseX < volume->getSizeX() - 1) && (baseY < volume->getSizeY() - 1) && (baseZ < volume->getSizeZ() - 1)) {
				Vector3DFloat dist = pt - Vector3DFloat(baseX, baseY, baseZ);
				Vector3DFloat a = forceField[volume->getIndex(baseX    , baseY    , baseZ    )];
				Vector3DFloat b = forceField[volume->getIndex(baseX + 1, baseY    , baseZ    )];
				Vector3DFloat c = forceField[volume->getIndex(baseX    , baseY + 1, baseZ    )];
				Vector3DFloat d = forceField[volume->getIndex(baseX + 1, baseY + 1, baseZ    )];
				Vector3DFloat e = forceField[volume->getIndex(baseX    , baseY    , baseZ + 1)];
				Vector3DFloat f = forceField[volume->getIndex(baseX + 1, baseY    , baseZ + 1)];
				Vector3DFloat g = forceField[volume->getIndex(baseX    , baseY + 1, baseZ + 1)];
				Vector3DFloat h = forceField[volume->getIndex(baseX + 1, baseY + 1, baseZ + 1)];

				Vector3DFloat f1 = a * (1.0-dist.X()) + b * dist.X();
				Vector3DFloat f2 = c * (1.0-dist.X()) + d * dist.X();
				Vector3DFloat f3 = e * (1.0-dist.X()) + f * dist.X();
				Vector3DFloat f4 = g * (1.0-dist.X()) + h * dist.X();

				Vector3DFloat f5 = f1 * (1.0-dist.Y()) + f2 * dist.Y();
				Vector3DFloat f6 = f3 * (1.0-dist.Y()) + f4 * dist.Y();

				return f5 * (1.0-dist.Z()) + f6 * dist.Z();
			} else {
				return Vector3DFloat(0,0,0);
			}
			 
		}

		Vector3DFloat SSEFlexibleFitter::GetForceOnHelix(Vector3DFloat p, Vector3DFloat q, double discretizationStepSize) {
			Vector3DFloat force = Vector3DFloat(0,0,0);
			for(double i = 0.0; i <= 1.0; i+= discretizationStepSize) {
				force = force + GetForceOnPoint(p * (1.0 - i) + q * i);
			}
			return force;
		}

		Vector3DFloat SSEFlexibleFitter::GetForceOnSheet(vector<Vector3DFloat> pts) {
			Vector3DFloat force = Vector3DFloat(0,0,0);
			for(unsigned int i = 0; i < pts.size(); i++) {
				force = force + GetForceOnPoint(pts[i]);
			}
			return force;
		}

		Vector3DFloat SSEFlexibleFitter::GetTorqueOnPoint(Vector3DFloat pt, Vector3DFloat center) {
			return ((pt - center) ^ GetForceOnPoint(pt));		
		}

		Vector3DFloat SSEFlexibleFitter::GetTorqueOnHelix(Vector3DFloat p, Vector3DFloat q, double discretizationStepSize) {
			Vector3DFloat torque = Vector3DFloat(0,0,0);
			Vector3DFloat center = (p + q) * 0.5;
			for(double i = 0.0; i <= 1.0; i+= discretizationStepSize) {
				torque = torque + GetTorqueOnPoint(p * (1.0 - i) + q * i, center);
			}
			return torque;
		}

		Vector3DFloat SSEFlexibleFitter::GetTorqueOnSheet(vector<Vector3DFloat> pts) {
			Vector3DFloat torque = Vector3DFloat(0,0,0);
			Vector3DFloat center = Vector3DFloat(0,0,0);
			
			for(unsigned int i = 0; i < pts.size(); i++) {
				center = center + pts[i] * (1.0f/(float)pts.size());
			}

			for(unsigned int i = 0; i < pts.size(); i++) {
				torque = torque + GetTorqueOnPoint(pts[i], center);
			}
			return torque;
		}


		void SSEFlexibleFitter::MoveHelix(GeometricShape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize) {
			Vector3DFloat p = WorldPointToVolume(helix->GetCornerCell3(1));
			Vector3DFloat q = WorldPointToVolume(helix->GetCornerCell3(2));

			Vector3DFloat torque = VolumeVectorToWorld(GetTorqueOnHelix(p, q, discretizationStepSize));			
			if(!isZero(torque.Length(), 0.0001)) {
				torque.Normalize();
				helix->Rotate(Vector3(torque.X(), torque.Y(), torque.Z()), rotationStepSize * 2 * PI);
			}		

			Vector3DFloat force = VolumeVectorToWorld(GetForceOnHelix(p, q, discretizationStepSize));
			if(!isZero(force.Length(), 0.0001)) {
				force = Vector3DFloat::Normalize(force) * translationStepSize;
				helix->Translate(Vector3(force.X(), force.Y(), force.Z()));
			}
			
			
		}


		void SSEFlexibleFitter::MoveSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize) {
			vector<Vector3DFloat> sheetPoints, trianglePoints;
			for(unsigned int j = 0; j < sheetMesh->faces.size(); j++) {
				if(sheetMesh->faces[j].tag.id == sheetId) {
					trianglePoints = sheetMesh->SampleTriangle(j, discretizationStepSize);
					for(unsigned int k = 0; k < trianglePoints.size(); k++) {
						sheetPoints.push_back(WorldPointToVolume(trianglePoints[k]));
					}							
				}
			}
			trianglePoints.clear();

			Vector3DFloat center = Vector3DFloat(0,0,0);
			
			for(unsigned int i = 0; i < sheetPoints.size(); i++) {
				center = center + sheetPoints[i] * (1.0f/(float)sheetPoints.size());
			}

			Vector3DFloat torque = VolumeVectorToWorld(GetTorqueOnSheet(sheetPoints));
			if(!isZero(torque.Length(), 0.0001)) {
				torque.Normalize();
			
				for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
					sheetMesh->vertices[i].tag = false;
				}

				unsigned int vertexIx;
				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
					if(sheetMesh->faces[i].tag.id == sheetId) {
						for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
							vertexIx = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
							NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[vertexIx]);
							if(!v->tag) {
								v->tag = true;
								v->position = v->position - center;
								v->position = v->position.Rotate(torque, rotationStepSize * 2 * PI);
								v->position = v->position + center;
							}
						}
					}
				}
			}


			Vector3DFloat force = VolumeVectorToWorld(GetForceOnSheet(sheetPoints));
			if(!isZero(force.Length(), 0.0001)) {
				force = Vector3DFloat::Normalize(force) * translationStepSize;
			
				for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
					sheetMesh->vertices[i].tag = false;
				}

				unsigned int vertexIx;
				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
					if(sheetMesh->faces[i].tag.id == sheetId) {
						for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
							vertexIx = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
							NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[vertexIx]);
							if(!v->tag) {
								v->tag = true;
								sheetMesh->TranslateVertex(vertexIx, force);
							}
						}
					}
				}
			} 

			sheetPoints.clear();			
		}

		void SSEFlexibleFitter::FitHelix(GeometricShape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount) {
			double t;
			for(int i = 0; i < maxStepCount; i++) {
				t = (double)(maxStepCount - i) / (double)maxStepCount;
				MoveHelix(helix, translationStepSize * t, rotationStepSize * t, discretizationStepSize);
			}
		}

		void SSEFlexibleFitter::FitSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount) {
			double t;
			for(int i = 0; i < maxStepCount; i++) {
				t = (double)(maxStepCount - i) / (double)maxStepCount;
				MoveSheet(sheetId, sheetMesh, translationStepSize * t, rotationStepSize * t, discretizationStepSize);
			}
		}
	}
}

#endif
