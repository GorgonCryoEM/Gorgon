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

        void EigenAnalysis(Eigen2D & in);
        void EigenAnalysis(Eigen3D & in);

    private:
        Matlab mathWrapper;
    };

    MathLib::MathLib()
        : mathWrapper(Matlab())
    {}

    void MathLib::EigenAnalysis(Eigen2D & in) {
        return mathWrapper.EigenAnalysis(in);
    }

    void MathLib::EigenAnalysis(Eigen3D & in) {
        return mathWrapper.EigenAnalysis(in);
    }
}

#endif
