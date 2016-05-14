/*
 * PDBAtom.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_GRAPHMATCH_PDBATOM_H_
#define SRC_TOOLKIT_GRAPHMATCH_PDBATOM_H_

#include <MathTools/Vector3.h>
#include <Foundation/StringUtils.h>
#include <string>
//#include <MathTools/Matrix.h>

//using namespace MathTools;
using namespace Foundation;
using namespace GraphMatch;

using namespace std;

namespace GraphMatch {

    static const int ATOM_ROLE_COUNT = 44;
    static const string ATOM_ROLE_NAMES[ATOM_ROLE_COUNT] = {
        "C",    "CA",   "CB",   "CD",   "CD1",  "CD2",  "CE",   "CE1",  "CE2",  "CE3",
        "CH",   "CH1",  "CH2",  "CG",   "CG1",  "CG2",  "CZ",   "CZ1",  "CZ2",  "CZ3",
        "N",    "ND1",  "ND2",  "NE",   "NE1",  "NE2",  "NH",   "NH1",  "NH2",  "NZ",
        "O",    "OD",   "OD1",  "OD2",  "OE",   "OE1",  "OE2",  "OG",   "OG1",  "OG2",
        "OH",   "OXT",  "SD",   "SG"
    };

    class PDBAtom {
        public:

            PDBAtom();
            PDBAtom(string pdbId, char chainId, unsigned int resSeq, string name);
            PDBAtom(string PDBLine, string pdbId = "----");
            void Print();

            string          GetPDBId();
            unsigned int    GetSerial();
            string          GetName();
            char            GetAltLoc();
            string          GetResName();
            char            GetChainId();
            unsigned int    GetResSeq();
            char            GetICode();
            Vec3F   GetPosition();
            float           GetOccupancy();
            float           GetTempFactor();
            string          GetElement();
            string          GetCharge();
            float           GetColorA();
            float           GetColorR();
            float           GetColorG();
            float           GetColorB();
            float           GetAtomRadius();
            bool            GetSelected();
            bool            GetVisible();
            unsigned long long  GetHashKey();
            unsigned long long  GetPrevCAHash(); // getting the Next and Previous hashes added
            unsigned long long  GetNextCAHash(); // for rendering purposes
            int             GetFlag();      // Purely for implementation purposes
            float           GetCorrelationScore();
            float           GetSkeletonScore();
            float           GetGeometryScore();
            float           GetTotalScore(float correlationWeight, float skeletonWeight, float geometryWeight);
            static unsigned long long ConstructHashKey(string pdbId, char chainId, unsigned int resSeq, string name);
            string          GetPDBString();

            void SetSerial(unsigned int serial);
            void SetName(string name);
            void SetAltLoc(char altLoc);
            void SetResName(string resName);
            void SetChainId(char chainId);
            void SetResSeq(unsigned int resSeq);
            void SetICode(char iCode);
            void SetPosition(Vec3F position);
            void SetOccupancy(float occupancy);
            void SetTempFactor(float tempFactor);
            void SetElement(string element);
            void SetCharge(string charge);
            void SetColor(float r, float g, float b, float a);
            void GetColor(float & r, float & g, float & b, float & a);
            void SetAtomRadius(float radius);
            void SetSelected(bool selected);
            void SetFlag(int flag);
            void SetVisible(bool visible);
            void SetCorrelationScore(float score);
            void SetSkeletonScore(float score);
            void SetGeometryScore(float score);
            void Transform(MatrixFloat transformMatrix);
            void InterpolateTransform(MatrixFloat transformMatrix1, MatrixFloat transformMatrix2, float coefficient);
            Vec3F GetInterpolateTransformLocation(MatrixFloat transformMatrix1, MatrixFloat transformMatrix2, float coefficient);
            void SetPrevCAHash(unsigned long long prevHash);  // previous and next CAs identifiable for rendering purposes
            void SetNextCAHash(unsigned long long nextHash);  // these are implemented naively rather than using a function
                                                              // to generate a hash code

        private:
            static unsigned long long GetCharIndex(char c);
            static unsigned long long GetPDBIdIndex(string pdbId);
            static unsigned long long GetChainIdIndex(char chainId);
            static unsigned long long GetAtomTypeIndex(string atomType);

        private:
            string          pdbId;
            unsigned int    serial;
            string          name;
            char            altLoc;
            string          resName;
            char            chainId;
            unsigned int    resSeq;
            char            iCode;
            Vec3F   position;
            float           occupancy;
            float           tempFactor;
            string          element;
            string          charge;
            float           atomRadius;
            float           colorR;
            float           colorG;
            float           colorB;
            float           colorA;
            bool            selected;
            bool            visible;
            int             flag;
            unsigned long long prevCAHash;
            unsigned long long nextCAHash;
            bool            prevWasSet;
            bool            nextWasSet;

            float           correlationScore;
            float           skeletonScore;
            float           geometryScore;
    };

} /* namespace GraphMatch */

#endif /* SRC_TOOLKIT_GRAPHMATCH_PDBATOM_H_ */
