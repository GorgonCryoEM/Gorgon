/*
 * Matlab.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_MATHTOOLS_MATLAB_H_
#define SRC_TOOLKIT_MATHTOOLS_MATLAB_H_

//#include <cstdio>
//#include <cstdlib>
//#include <string>
#include "DataStructures.h"
//#include "Vector3.h"
//#include "ComplexNumber.h"
#include <schaefer_eigen/Eigen.h>
//#include <cmath>
#ifdef USE_MATLAB
#include "engine.h"
#endif

namespace MathTools {
    class Matlab {

    /*
     *
     */
    public:
        Matlab();
        ~Matlab();
        void EigenAnalysis(Eigen2D & in);
        void EigenAnalysis(Eigen3D & in);
    private:
        #ifdef USE_MATLAB
        Engine * mathEngine;
        #endif
    };

} /* namespace MathTools */

#endif /* SRC_TOOLKIT_MATHTOOLS_MATLAB_H_ */
