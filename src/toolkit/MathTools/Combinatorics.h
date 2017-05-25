#ifndef TOOLKIT_MATHTOOLS_COMBINATORICS_H
#define TOOLKIT_MATHTOOLS_COMBINATORICS_H

#include "DataStructures.h"
#include "Dim3D.h"
#include <algorithm>

using namespace std;
using namespace Toolkit;

namespace MathTools {
    static unsigned long long Combinations(int n, int r);
    static unsigned long long Permutations(int n, int r);

    static void BinomDistr(ProbDistr1D & distro);
    static void BinomDistr(ProbDistr2D & distro);
    static void BinomDistr(ProbDistr3D & distro);
    static void UniformDistr(ProbDistr3D & distro);
    static void AnisoDistrAxisAligned(ProbDistr3D & distr, int xR, int yR, int zR);

    inline unsigned long long Combinations(int n, int r) {
        long long c = 1;
        for(int i = r+1; i <= n; i++) {
            c = c * i;
        }
        for(int i = 2; i <= (n-r); i++) {
            c = c / i;
        }
        return c;
    }

    inline unsigned long long Permutations(int n, int r) {
        long long p = 1;
        for(int i = n-r+1; i <= n; i++) {
            p = p * i;
        }
        return p;
    }

    inline void BinomDistr(ProbDistr1D & distro) {
        double total = 0;

        for(int x = -distro.R, i=0; x <= distro.R; x++, i++) {
            distro.vals[i] = (float)Combinations(2 * distro.R, x+distro.R);
            total += distro.vals[i];
        }

        for(int x = -distro.R, i=0; x <= distro.R; x++, i++) {
            distro.vals[i] = (float)(distro.vals[i] / total);
        }
    }

    inline void BinomDistr(ProbDistr2D & distro) {
        ProbDistr1D dist1D;
        dist1D.R = distro.R;
        BinomDistr(dist1D);
        for(int x = 0; x < dist1D.R * 2 +1; x++) {
            for(int y = 0; y < dist1D.R * 2 +1; y++) {
                distro.vals[x][y] = dist1D.vals[x] * dist1D.vals[y];
            }
        }
    }

    inline void BinomDistr(ProbDistr3D & distro) {
        ProbDistr1D dist1D;
        dist1D.R = distro.R;
        BinomDistr(dist1D);
        for(int x = 0; x < dist1D.R * 2 +1; x++) {
            for(int y = 0; y < dist1D.R * 2 +1; y++) {
                for(int z = 0; z < dist1D.R * 2 +1; z++) {
                    distro.vals[x][y][z] = dist1D.vals[x] * dist1D.vals[y] * dist1D.vals[z];
                }
            }
        }
    }

    inline void UniformDistr(ProbDistr3D & distro) {
        double probability = 1.0 / (double)(distro.R * 2 +1) * (distro.R * 2 +1) * (distro.R * 2 +1);
        for(int x = 0; x < distro.R * 2 +1; x++) {
            for(int y = 0; y < distro.R * 2 +1; y++) {
                for(int z = 0; z < distro.R * 2 +1; z++) {
                    distro.vals[x][y][z] = (float)probability;
                }
            }
        }
    }

    inline void AnisoDistrAxisAligned(ProbDistr3D & distr, int xR, int yR, int zR) {
        distr.R = max (xR, max(yR, zR));

        Dim3D<int> R(xR, yR, zR);
        Dim3D<ProbDistr1D> BinomialDist;

        BinomialDist[0].R = R[0];        BinomDistr(BinomialDist[0]);
        BinomialDist[1].R = R[1];        BinomDistr(BinomialDist[1]);
        BinomialDist[2].R = R[2];        BinomDistr(BinomialDist[2]);

        for(int x = 0; x < distr.R * 2 +1; x++) {
            for(int y = 0; y < distr.R * 2 +1; y++) {
                for(int z = 0; z < distr.R * 2 +1; z++) {
                    if((x < distr.R-R.X()) || (x > distr.R+R.X()) ||
                       (y < distr.R-R.Y()) || (y > distr.R+R.Y()) ||
                       (z < distr.R-R.Z()) || (z > distr.R+R.Z())) {
                       distr.vals[x][y][z] = 0;
                    } else {
                        distr.vals[x][y][z] = BinomialDist[0].vals[x - distr.R + R.X()] *
                                                 BinomialDist[1].vals[y - distr.R + R.Y()] *
                                                 BinomialDist[2].vals[z - distr.R + R.Z()];
                    }
                }
            }
        }
    }
}

#endif
