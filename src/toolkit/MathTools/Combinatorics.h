#ifndef TOOLKIT_MATHTOOLS_COMBINATORICS_H
#define TOOLKIT_MATHTOOLS_COMBINATORICS_H

#include "DataStructures.h"
#include <algorithm>

using namespace std;

namespace MathTools {
    static unsigned long long Combinations(int n, int r);
    static unsigned long long Permutations(int n, int r);

    static void GetBinomialDistribution(ProbabilityDistribution1D & distro);
    static void GetBinomialDistribution(ProbabilityDistribution2D & distro);
    static void GetBinomialDistribution(ProbabilityDistribution3D & distro);
    static void GetUniformDistribution(ProbabilityDistribution3D & distro);
    static void GetAnisotropicDistributionAxisAligned(ProbabilityDistribution3D & distro, int xR, int yR, int zR);

    unsigned long long Combinations(int n, int r) {
        long long c = 1;
        for(int i = r+1; i <= n; i++) {
            c = c * i;
        }
        for(int i = 2; i <= (n-r); i++) {
            c = c / i;
        }
        return c;
    }

    unsigned long long Permutations(int n, int r) {
        long long p = 1;
        for(int i = n-r+1; i <= n; i++) {
            p = p * i;
        }
        return p;
    }

    void GetBinomialDistribution(ProbabilityDistribution1D & distro) {
        double total = 0;

        for(int x = -distro.radius, i=0; x <= distro.radius; x++, i++) {
            distro.values[i] = (float)Combinations(2 * distro.radius, x+distro.radius);
            total += distro.values[i];
        }

        for(int x = -distro.radius, i=0; x <= distro.radius; x++, i++) {
            distro.values[i] = (float)(distro.values[i] / total);
        }
    }

    void GetBinomialDistribution(ProbabilityDistribution2D & distro) {
        ProbabilityDistribution1D dist1D;
        dist1D.radius = distro.radius;
        GetBinomialDistribution(dist1D);
        for(int x = 0; x < dist1D.radius * 2 +1; x++) {
            for(int y = 0; y < dist1D.radius * 2 +1; y++) {
                distro.values[x][y] = dist1D.values[x] * dist1D.values[y];
            }
        }
    }

    void GetBinomialDistribution(ProbabilityDistribution3D & distro) {
        ProbabilityDistribution1D dist1D;
        dist1D.radius = distro.radius;
        GetBinomialDistribution(dist1D);
        for(int x = 0; x < dist1D.radius * 2 +1; x++) {
            for(int y = 0; y < dist1D.radius * 2 +1; y++) {
                for(int z = 0; z < dist1D.radius * 2 +1; z++) {
                    distro.values[x][y][z] = dist1D.values[x] * dist1D.values[y] * dist1D.values[z];
                }
            }
        }
    }

    void GetUniformDistribution(ProbabilityDistribution3D & distro) {
        double probability = 1.0 / (double)(distro.radius * 2 +1) * (distro.radius * 2 +1) * (distro.radius * 2 +1);
        for(int x = 0; x < distro.radius * 2 +1; x++) {
            for(int y = 0; y < distro.radius * 2 +1; y++) {
                for(int z = 0; z < distro.radius * 2 +1; z++) {
                    distro.values[x][y][z] = (float)probability;
                }
            }
        }
    }

    void GetAnisotropicDistributionAxisAligned(ProbabilityDistribution3D & distro, int xR, int yR, int zR) {
        distro.radius = max (xR, max(yR, zR));
        ProbabilityDistribution1D xBinomialDist, yBinomialDist, zBinomialDist;

        xBinomialDist.radius = xR;
        GetBinomialDistribution(xBinomialDist);
        yBinomialDist.radius = yR;
        GetBinomialDistribution(yBinomialDist);
        zBinomialDist.radius = zR;
        GetBinomialDistribution(zBinomialDist);

        for(int x = 0; x < distro.radius * 2 +1; x++) {
            for(int y = 0; y < distro.radius * 2 +1; y++) {
                for(int z = 0; z < distro.radius * 2 +1; z++) {
                    if((x < distro.radius-xR) || (x > distro.radius+xR) ||
                        (y < distro.radius-yR) || (y > distro.radius+yR) ||
                        (z < distro.radius-zR) || (z > distro.radius+zR)) {
                        distro.values[x][y][z] = 0;
                    } else {
                        distro.values[x][y][z] = xBinomialDist.values[x - distro.radius + xR] *
                            yBinomialDist.values[y - distro.radius + yR] *
                            zBinomialDist.values[z - distro.radius + zR];
                    }
                }
            }
        }
    }
}

#endif
