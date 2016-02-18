#ifndef TOOLKIT_MATHTOOLS_MATH_LIB_H
#define TOOLKIT_MATHTOOLS_MATH_LIB_H

#include "Matlab.h"
#include "Combinatorics.h"
//#include "Vector3D.h"
//#include "ComplexNumber.h"


namespace MathTools {

    const double PI = 3.1415926535897932385;

    class MathLib {
    public:
        MathLib();
        ~MathLib();

        void EigenAnalysis           (EigenVectorsAndValues2D   & eigenInformation);
        void EigenAnalysis           (EigenVectorsAndValues3D   & eigenInformation);
        void GetBinomialDistribution (ProbabilityDistribution1D & distributionInfo);
        void GetBinomialDistribution (ProbabilityDistribution2D & distributionInfo);
        void GetBinomialDistribution (ProbabilityDistribution3D & distributionInfo);
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

    void MathLib::GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo) {
        return Combinatorics::GetBinomialDistribution(distributionInfo);
    }

    void MathLib::GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo) {
        return Combinatorics::GetBinomialDistribution(distributionInfo);
    }

    void MathLib::GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo) {
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
