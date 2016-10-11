//
// Created by shadow_walker on 10/10/16.
//

#ifndef SRC_TOOLKIT_CORE_GORGON_ROUND_H_
#define SRC_TOOLKIT_CORE_GORGON_ROUND_H_

inline double round(double f){
    return (f >= 0?(int)(f + .5):(int)(f - .5));
}

#endif //GORGON_ROUND_H
