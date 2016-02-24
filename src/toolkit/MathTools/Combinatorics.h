#ifndef TOOLKIT_MATHTOOLS_COMBINATORICS_H
#define TOOLKIT_MATHTOOLS_COMBINATORICS_H

#include "DataStructures.h"
#include <algorithm>

using namespace std;

namespace MathTools {
    static unsigned long long Combinations(int n, int r);
    static unsigned long long Permutations(int n, int r);

    static void GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo);
    static void GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo);
    static void GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo);
    static void GetUniformDistribution(ProbabilityDistribution3D & distributionInfo);
    static void GetAnisotropicDistributionAxisAligned(ProbabilityDistribution3D & distributionInfo, int xRadius, int yRadius, int zRadius);

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

    void GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo) {
        double total = 0;

        for(int x = -distributionInfo.radius, i=0; x <= distributionInfo.radius; x++, i++) {
            distributionInfo.values[i] = (float)Combinations(2 * distributionInfo.radius, x+distributionInfo.radius);
            total += distributionInfo.values[i];
        }

        for(int x = -distributionInfo.radius, i=0; x <= distributionInfo.radius; x++, i++) {
            distributionInfo.values[i] = (float)(distributionInfo.values[i] / total);
        }
    }

    void GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo) {
        ProbabilityDistribution1D dist1D;
        dist1D.radius = distributionInfo.radius;
        GetBinomialDistribution(dist1D);
        for(int x = 0; x < dist1D.radius * 2 +1; x++) {
            for(int y = 0; y < dist1D.radius * 2 +1; y++) {
                distributionInfo.values[x][y] = dist1D.values[x] * dist1D.values[y];
            }
        }
    }

    void GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo) {
        ProbabilityDistribution1D dist1D;
        dist1D.radius = distributionInfo.radius;
        GetBinomialDistribution(dist1D);
        for(int x = 0; x < dist1D.radius * 2 +1; x++) {
            for(int y = 0; y < dist1D.radius * 2 +1; y++) {
                for(int z = 0; z < dist1D.radius * 2 +1; z++) {
                    distributionInfo.values[x][y][z] = dist1D.values[x] * dist1D.values[y] * dist1D.values[z];
                }
            }
        }
    }

    void GetUniformDistribution(ProbabilityDistribution3D & distributionInfo) {
        double probability = 1.0 / (double)(distributionInfo.radius * 2 +1) * (distributionInfo.radius * 2 +1) * (distributionInfo.radius * 2 +1);
        for(int x = 0; x < distributionInfo.radius * 2 +1; x++) {
            for(int y = 0; y < distributionInfo.radius * 2 +1; y++) {
                for(int z = 0; z < distributionInfo.radius * 2 +1; z++) {
                    distributionInfo.values[x][y][z] = (float)probability;
                }
            }
        }
    }

    void GetAnisotropicDistributionAxisAligned(ProbabilityDistribution3D & distributionInfo, int xRadius, int yRadius, int zRadius) {
        distributionInfo.radius = max (xRadius, max(yRadius, zRadius));
        ProbabilityDistribution1D xBinomialDist, yBinomialDist, zBinomialDist;

        xBinomialDist.radius = xRadius;
        GetBinomialDistribution(xBinomialDist);
        yBinomialDist.radius = yRadius;
        GetBinomialDistribution(yBinomialDist);
        zBinomialDist.radius = zRadius;
        GetBinomialDistribution(zBinomialDist);

        for(int x = 0; x < distributionInfo.radius * 2 +1; x++) {
            for(int y = 0; y < distributionInfo.radius * 2 +1; y++) {
                for(int z = 0; z < distributionInfo.radius * 2 +1; z++) {
                    if((x < distributionInfo.radius-xRadius) || (x > distributionInfo.radius+xRadius) ||
                        (y < distributionInfo.radius-yRadius) || (y > distributionInfo.radius+yRadius) ||
                        (z < distributionInfo.radius-zRadius) || (z > distributionInfo.radius+zRadius)) {
                        distributionInfo.values[x][y][z] = 0;
                    } else {
                        distributionInfo.values[x][y][z] = xBinomialDist.values[x - distributionInfo.radius + xRadius] *
                            yBinomialDist.values[y - distributionInfo.radius + yRadius] *
                            zBinomialDist.values[z - distributionInfo.radius + zRadius];
                    }
                }
            }
        }
    }
}

#endif
