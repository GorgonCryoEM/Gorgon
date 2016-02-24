#ifndef TOOLKIT_MATHTOOLS_MATH_LIB_H
#define TOOLKIT_MATHTOOLS_MATH_LIB_H

#include "Matlab.h"
#include "Combinatorics.h"
//#include "Vector3.h"
//#include "ComplexNumber.h"


namespace MathTools {

    const double PI = 3.1415926535897932385;

    class MathLib {
    public:
        MathLib();
        ~MathLib();

        void EigenAnalysis           (EigenVectorsAndValues2D   & eigenInformation);
        void EigenAnalysis           (EigenVectorsAndValues3D   & eigenInformation);
        template <class T>
        void GetBinomialDistribution (T & distributionInfo);
        void GetUniformDistribution  (ProbabilityDistribution3D & distributionInfo);
        unsigned long long Combinations(int n, int r);
        unsigned long long Permutations(int n, int r);

    private:
        Matlab * mathWrapper;
    };

    MathLib::MathLib() {
        mathWrapper = new Matlab();
    }

    MathLib::~MathLib() {
        delete mathWrapper;
    }

    void MathLib::EigenAnalysis(EigenVectorsAndValues2D & eigenInformation) {
        return mathWrapper->EigenAnalysis(eigenInformation);
    }

    void MathLib::EigenAnalysis(EigenVectorsAndValues3D & eigenInformation) {
        #ifdef USE_MATLAB
        return mathWrapper->EigenAnalysisMatlab(eigenInformation);
        #else
        return mathWrapper->EigenAnalysis(eigenInformation);
        #endif
    }

    template <class T>
    void MathLib::GetBinomialDistribution(T & distributionInfo) {
        return Combinatorics::GetBinomialDistribution(distributionInfo);
    }

    void MathLib::GetUniformDistribution(ProbabilityDistribution3D & distributionInfo) {
        return Combinatorics::GetUniformDistribution(distributionInfo);
    }

    unsigned long long MathLib::Combinations(int n, int r) {
        return Combinatorics::Combinations(n, r);
    }

    unsigned long long MathLib::Permutations(int n, int r) {
        return Combinatorics::Permutations(n, r);
    }
}

#endif
