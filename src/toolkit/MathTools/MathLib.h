#ifndef TOOLKIT_MATHTOOLS_MATH_LIB_H
#define TOOLKIT_MATHTOOLS_MATH_LIB_H

#include "Matlab.h"
//#include "Vector3.h"
//#include "ComplexNumber.h"


namespace MathTools {

    const double PI = 3.1415926535897932385;

    class MathLib {
    public:
        MathLib();

        void EigenAnalysis           (Eigen2D   & eigenInformation);
        void EigenAnalysis           (Eigen3D   & eigenInformation);

    private:
        Matlab mathWrapper;
    };

    MathLib::MathLib()
        : mathWrapper(Matlab())
    {}

    void MathLib::EigenAnalysis(Eigen2D & eigenInformation) {
        return mathWrapper.EigenAnalysis(eigenInformation);
    }

    void MathLib::EigenAnalysis(Eigen3D & eigenInformation) {
        #ifdef USE_MATLAB
        return mathWrapper.EigenAnalysisMatlab(eigenInformation);
        #else
        return mathWrapper.EigenAnalysis(eigenInformation);
        #endif
    }
}

#endif
