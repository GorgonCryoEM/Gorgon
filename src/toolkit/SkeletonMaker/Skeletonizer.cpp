/*
 * Skeletonizer.cpp
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Skeletonizer.h"

namespace GraySkeletonCPP {


    const char Skeletonizer::THINNING_CLASS_SURFACE_PRESERVATION = 4;
    const char Skeletonizer::THINNING_CLASS_CURVE_PRESERVATION_2D = 3;
    const char Skeletonizer::THINNING_CLASS_CURVE_PRESERVATION = 2;
    const char Skeletonizer::THINNING_CLASS_POINT_PRESERVATION = 1;
    const char Skeletonizer::THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
    const char Skeletonizer::PRUNING_CLASS_PRUNE_SURFACES = 5;
    const char Skeletonizer::PRUNING_CLASS_PRUNE_CURVES = 6;
    const char Skeletonizer::PRUNING_CLASS_PRUNE_POINTS = 7;


    Skeletonizer::Skeletonizer(int pointR, int curveR, int surfaceR, int skelDirR)
        : math(Matlab()),
          surfNormFinder(NormalFinder())
    {
        this->pointR = pointR;
        this->curveR = curveR;
        this->surfR = surfaceR;
        this->skelDirR = skelDirR;

        gaussFiltPtR.R = pointR;
        BinomDistr(gaussFiltPtR);

        gaussFiltCrvR.R = curveR;
        BinomDistr(gaussFiltCrvR);

        gaussFiltSrfcR.R = surfaceR;
        BinomDistr(gaussFiltSrfcR);

        gaussFiltMaxR.R = MAX_GAUSS_FILT_R;
        BinomDistr(gaussFiltMaxR);

        uniformFiltSkelDirR.R = skelDirR;
        UniformDistr(uniformFiltSkelDirR);
    }

    double Skeletonizer::GetVoxelCost(EigenResults3D imgEigen, Vec3F skelDir, int type) {
        double cost = 1;

        if(!isZero(imgEigen.vals[0])) {
            double theta, a, b;
            Vec3F temp, skelDirectionST, n;
            float u1 = 1.0;
            float u2 = abs(imgEigen.vals[1]/imgEigen.vals[0]);
            float u3 = abs(imgEigen.vals[2]/imgEigen.vals[0]);
            Vec3F v1 = imgEigen.vecs[0];
            Vec3F v2 = imgEigen.vecs[1];
            Vec3F v3 = imgEigen.vecs[2];
            switch(type) {
                case PRUNING_CLASS_PRUNE_POINTS:

                    cost = u3*u3 / (u1*u2);
                    break;
                case PRUNING_CLASS_PRUNE_CURVES:

                    if(skelDir.isBadNormal()) {
                        cost = 1.0;
                    } else {
                        n = XYZtoUVW(skelDir, v1, v2, v3);
                        a = u1 * u2 * u3;
                        b = sqrt(u2*u2*u3*u3*n.X()*n.X() + u1*u1*u3*u3*n.Y()*n.Y() + u1*u1*u2*u2*n.Z()*n.Z());
                        temp = n*(a/b);
                        cost = u3/ temp.length();
                    }


                    break;
                case PRUNING_CLASS_PRUNE_SURFACES:

                    {
                        if(skelDir.isBadNormal()) {
                            cost = 1.0;
                        } else {
                            Vec3F n1, n2, m1, m2;
                            skelDirectionST = XYZtoUVW(skelDir, imgEigen.vecs[0],imgEigen.vecs[1], imgEigen.vecs[2]);
                            FindOrthogonalAxes(skelDirectionST, n1, n2);

                            m1 = Vec3F(n1[0]/u1, n1[1]/u2, n1[2]/u3);
                            m2 = Vec3F(n2[0]/u1, n2[1]/u2, n2[2]/u3);
                            theta = atan((2.0 * (m1 * m2)) / ((m1 * m1) - (m2 * m2))) / 2.0;
                            a = 1.0 / ((m1 * cos(theta)) + (m2 * sin(theta))).length();
                            b = 1.0 / ((m1 * sin(theta)) - (m2 * cos(theta))).length();
                            cost = (u2 * u3) / (a*b);
                        }
                    }
                    break;
            }

        } else {
            cost = 1.0;
            //printf("Zero\n");
        }

        if(cost != cost) {
            cost = 1.0;
        }
        return cost;

    }

    vector<EigenResults3D> Skeletonizer::GetEigenResults(
            const Volume & mask, vector<Vec3F> & imgGrad,
            ProbDistr3D & gaussFilt, int gaussFiltR,
            bool useMask)
    {
        vector<EigenResults3D> resultTable(mask.getSize());

        for(int x = MAX_GAUSS_FILT_R; x < mask.getSizeX() - MAX_GAUSS_FILT_R; x++) {
            for(int y = MAX_GAUSS_FILT_R; y < mask.getSizeY() - MAX_GAUSS_FILT_R; y++) {
                for(int z = MAX_GAUSS_FILT_R; z < mask.getSizeZ() - MAX_GAUSS_FILT_R; z++) {
                    GetEigenResult(resultTable[mask.getIndex(x, y, z)], imgGrad, gaussFilt, x, y, z,
                            mask.getSizeX(), mask.getSizeY(), mask.getSizeZ(), gaussFiltR, (useMask && (mask(x, y, z) == 0)));
                }
            }
        }

        return resultTable;
    }

    Vec3F Skeletonizer::XYZtoUVW(Vec3F vec, Vec3F u, Vec3F v, Vec3F w) {
        float uContri = vec * u;
        float vContri = vec * v;
        float wContri = vec * w;
        Vec3F inUVW = Vec3F(uContri, vContri, wContri);
        inUVW.normalize();
        return inUVW;
    }


    Vec3F Skeletonizer::GetCurveDirection(const Volume & skel, int x, int y, int z, int R) {
        Vec3F direction = Vec3F(0,0,0);
        if(skel.getN6Count(x, y, z) > 2) {
            direction = Vec3F(BAD_NORMAL, BAD_NORMAL, BAD_NORMAL);
        } else {

            int marg = 2;
            int size = (R+marg)*2 + 1;
            Volume block(size, size, size);
            for(int xx = marg; xx <= size-marg; xx++) {
                for(int yy = marg; yy <= size-marg; yy++) {
                    for(int zz = marg; zz <= size-marg; zz++) {
                        block(xx, yy, zz) = skel(x-R-marg+xx, y-R-marg+yy, z-R-marg+zz);
                    }
                }
            }

            Volume visited(size, size, size);

            vector<Vec3I> list;
            list.push_back(Vec3I(marg+R, marg+R, marg+R));
            Vec3I pos;
            vector<Vec3I> n6;
            int n6Count;

            while(list.size() > 0) {
                pos = list[list.size()-1];
                list.pop_back();
                visited(pos.X(), pos.Y(), pos.Z()) = 1;
                n6Count = block.getN6(n6, pos.X(), pos.Y(), pos.Z());

                if(skel.getN6Count(x+pos.X()-marg-R, y+pos.Y()-marg-R, z+pos.Z()-marg-R) <= 2) {
                    for(int i = 0; i < n6Count; i++) {
                        if(visited(n6[i].X(), n6[i].Y(), n6[i].Z()) < 1) {
                            list.push_back(n6[i]);
                        }
                    }
                }
            }

            vector<Vec3F> gradient = GetVolumeGradient(visited);
            EigenResults3D eigen;
            GetEigenResult(eigen, gradient, uniformFiltSkelDirR,
                marg+R, marg+R, marg+R,
                size, size, size, R, false);

            direction = eigen.vecs[2];
        }

        return direction;
    }

    Vec3F Skeletonizer::GetSurfaceNormal(const Volume & skel, int x, int y, int z) {
        surfNormFinder.InitializeGraph(skel, x, y, z);
        return surfNormFinder.GetSurfaceNormal();
    }


    Vec3F Skeletonizer::GetSurfaceNormal(const Volume & skel, int x, int y, int z, int R, vector<Vec3F> & localDirs) {
        Vec3F direction = localDirs[skel.getIndex(x, y, z)];


        if(!direction.isBadNormal()) {
            int margin = 2;
            int size = (R+margin)*2 + 1;
            Volume block(size, size, size);

            for(int xx = margin; xx <= size-margin; xx++) {
                for(int yy = margin; yy <= size-margin; yy++) {
                    for(int zz = margin; zz <= size-margin; zz++) {
                        block(xx, yy, zz) = skel(x-R-margin+xx, y-R-margin+yy, z-R-margin+zz);
                    }
                }
            }

            Volume visited(size, size, size);

            vector<Vec3I> list;
            list.push_back(Vec3I(margin+R, margin+R, margin+R));
            Vec3I currentPos, newPos;
            Vec3F tempDir;

            while(list.size() > 0) {
                currentPos = list[list.size()-1];
                list.pop_back();
                visited(currentPos.X(), currentPos.Y(), currentPos.Z()) = 1;
                tempDir = localDirs[skel.getIndex(x+currentPos.X()-margin-R, y+currentPos.Y()-margin-R, z+currentPos.Z()-margin-R)];

                if(!tempDir.isBadNormal()) {
                    for(int i = 0; i < 12; i++) {
                        if((block(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][0][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][0][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][0][2]) > 0) &&
                           (block(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][1][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][1][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][1][2]) > 0) &&
                           (block(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][2][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][2][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][2][2]) > 0)) {
                            for(int j = 0; j < 3; j++) {
                                newPos = currentPos + Vec3I(VOLUME_NEIGHBOR_FACES[i][j][0], VOLUME_NEIGHBOR_FACES[i][j][1], VOLUME_NEIGHBOR_FACES[i][j][2]);
                                if(visited(newPos.X(), newPos.Y(), newPos.Z()) < 1) {
                                    list.push_back(newPos);
                                }
                            }
                        }
                    }
                }
            }


            vector<Vec3F> gradient = GetVolumeGradient(visited);

            EigenResults3D eigen;

            GetEigenResult(eigen, gradient, uniformFiltSkelDirR,
                margin+R, margin+R, margin+R,
                size, size, size, R, false);

            direction = eigen.vecs[0];

        }

        return direction;
    }
    // Gradient = (x+1,y,z) - (x-1,y,z) ....
    vector<Vec3F> Skeletonizer::GetVolumeGradient(const Volume & src) {
        vector<Vec3F> gradient(src.getSizeX() * src.getSizeY() * src.getSizeZ());
        int index;

        for(int x = 0; x < src.getSizeX(); x = x + src.getSizeX()-1) {
            for(int y = 0; y < src.getSizeY(); y = y + src.getSizeY()-1) {
                for(int z = 0; z < src.getSizeZ(); z = z + src.getSizeZ()-1) {
                    index = src.getIndex(x, y, z);
                    gradient[index] = Vec3F(0, 0, 0);
                }
            }
        }
        Vec3F grad1, grad2;

        for(int x = 1; x < src.getSizeX()-1; x++) {
            for(int y = 1; y < src.getSizeY()-1; y++) {
                for(int z = 1; z < src.getSizeZ()-1; z++) {
                    index = src.getIndex(x, y, z);
                    grad1 = Vec3F(src(x, y, z) - src(x-1, y, z),
                                               src(x, y, z) - src(x, y-1, z),
                                               src(x, y, z) - src(x, y, z-1));

                    grad2 = Vec3F(src(x+1, y, z) - src(x, y, z),
                                               src(x, y+1, z) - src(x, y, z),
                                               src(x, y, z+1) - src(x, y, z));


                    for(int i = 0; i < 3; i++) {
                        if(abs(grad1[i]) > abs(grad2[i])) {
                            gradient[index][i] = grad1[i];
                        } else {
                            gradient[index][i] = grad2[i];
                        }
                    }
                }
            }
        }
        return gradient;
    }

    vector<Vec3F> Skeletonizer::GetSkeletonDirection(const Volume & skel, int type) {
        int index;

        vector<Vec3F> localDirs(skel.getSizeX() * skel.getSizeY() * skel.getSizeZ());

        for(int x = 1; x < skel.getSizeX()-1; x++) {
            for(int y = 1; y < skel.getSizeY()-1; y++) {
                for(int z = 1; z < skel.getSizeZ()-1; z++) {
                    index = skel.getIndex(x, y, z);
                    localDirs[index] = Vec3F(0,0,0);
                    if(skel(x,y,z) > 0) {
                        switch(type){
                            case PRUNING_CLASS_PRUNE_SURFACES:
                                localDirs[index] = GetSurfaceNormal(skel, x, y, z);
                                break;
                        }
                    }
                }
            }
        }

        vector<Vec3F> directions(skel.getSizeX() * skel.getSizeY() * skel.getSizeZ());

        for(int x = 1; x < skel.getSizeX()-1; x++) {
            for(int y = 1; y < skel.getSizeY()-1; y++) {
                for(int z = 1; z < skel.getSizeZ()-1; z++) {
                    index = skel.getIndex(x, y, z);
                    directions[index] = Vec3F(0,0,0);
                    if(skel(x,y,z) > 0) {
                        switch(type){
                            case PRUNING_CLASS_PRUNE_CURVES:
                                directions[index] = GetCurveDirection(skel, x, y, z, skelDirR);
                                break;
                            case PRUNING_CLASS_PRUNE_SURFACES:
                                directions[index] = GetSurfaceNormal(skel, x, y, z, skelDirR, localDirs);
                                break;
                        }
                    }
                }
            }
        }
        return directions;
    }


    void Skeletonizer::FindOrthogonalAxes(Vec3F axis, Vec3F & res1, Vec3F & res2) {
        res1 = Vec3F(1.0, 0.0, 0.0);
        if(abs(axis * res1) > 0.95) {
            res1 = Vec3F(0.0, 1.0, 0.0);
        }
        res1 = axis ^ res1;
        res2 = axis ^ res1;
        res1.normalize();
        res2.normalize();
    }

    void Skeletonizer::GetEigenResult(
                            EigenResults3D & returnVal,
                            vector<Vec3F> & imgGrad,
                            ProbDistr3D & gaussFilt,
                            int x, int y, int z,
                            int sizeX, int sizeY, int sizeZ,
                            int gaussFiltR, bool clear)
    {
        if(clear) {
            for(int r = 0; r < 3; r++) {
                returnVal.vals[r] = 0;
                for(int c = 0; c < 3; c++) {
                    returnVal.vecs[r][c] = 0;
                }
            }
        } else {
            Eigen3D eigenData;
            double probability;
            int index2;

            for(int r = 0; r < 3; r++) {
                for(int c = 0; c < 3; c++) {
                    eigenData.tensor[r][c] = 0;
                }
            }

            for(int xx = -gaussFiltR; xx <= gaussFiltR; xx++) {
                for(int yy = -gaussFiltR; yy <= gaussFiltR; yy++) {
                    for(int zz = -gaussFiltR; zz <= gaussFiltR; zz++) {
                        index2 = (x+xx) * sizeY * sizeZ + (y+yy) * sizeZ + z + zz;
                        probability = gaussFilt.vals[xx+gaussFiltR][yy+gaussFiltR][zz+gaussFiltR];
                        for(int r = 0; r < 3; r++) {
                            for(int c = 0; c < 3; c++) {
                                eigenData.tensor[r][c] += imgGrad[index2][r] * imgGrad[index2][c] * probability;
                            }
                        }
                    }
                }
            }

            math.EigenAnalysis(eigenData);
            for(int r = 0; r < 3; r++) {
                returnVal.vals[r] = eigenData.vals[r];
                for(int c = 0; c < 3; c++) {
                    returnVal.vecs[r][c] = eigenData.vecs[r][c];
                }
            }

            assert((returnVal.vals[0] >= returnVal.vals[1]) && (returnVal.vals[1] >= returnVal.vals[2]));


        }
    }


    void Skeletonizer::GetSTBasedDistribution(ProbDistr3D & distr, EigenResults3D eigen) {
        Vec3F skelDir;
        double total = 0;
        double cell;
        for(int x = -distr.R; x <= distr.R; x++) {
            for(int y = -distr.R; y <= distr.R; y++) {
                for(int z = -distr.R; z <= distr.R; z++) {
                    if((x!=0) && (y!=0) && (z!=0)) {
                        skelDir = Vec3F(0,0,0) - Vec3F(x, y, z);
                        skelDir.normalize();
                        cell = GetVoxelCost(eigen, skelDir, PRUNING_CLASS_PRUNE_CURVES);
                        distr.vals[x+distr.R][y+distr.R][z+distr.R] = cell;
                        total += cell;
                    }
                }
            }
        }

        for(int x = -distr.R; x <= distr.R; x++) {
            for(int y = -distr.R; y <= distr.R; y++) {
                for(int z = -distr.R; z <= distr.R; z++) {
                    distr.vals[x+distr.R][y+distr.R][z+distr.R] =
                        distr.vals[x+distr.R][y+distr.R][z+distr.R] / total;
                }
            }
        }

    }

    void Skeletonizer::PruneCurves(Volume & src, int pruneLength) {
        src.erodeHelix(pruneLength);
    }

    void Skeletonizer::PruneSurfaces(Volume & src, int pruneLength) {
        src.erodeSheet(pruneLength);
    }

    void Skeletonizer::PruneUsingStructureTensor(
            Volume &  skel, const Volume & src, Volume * preserved,
            vector<Vec3F> & volGrad, vector<EigenResults3D> & volumeEigens,
            ProbDistr3D & filter, double threshold, char pruningClass,
            string outPath)
    {
        Volume tempSkel(skel);
        Volume costVol(skel.getSizeX(), skel.getSizeY(), skel.getSizeZ());
        vector<Vec3F> skelDirs = GetSkeletonDirection(skel, pruningClass);
        int index;
        double cost;
        EigenResults3D eigen;

        for(int x = 0; x < skel.getSizeX(); x++) {
            for(int y = 0; y < skel.getSizeY(); y++) {
                for(int z = 0; z < skel.getSizeZ(); z++) {
                    index = skel.getIndex(x, y, z);
                    if(((preserved == NULL) || ((preserved != NULL) && (*preserved)(x, y, z) < 0.5)) && (tempSkel(x, y, z) > 0)) {
                        if(volumeEigens.empty()) {
                            GetEigenResult(eigen, volGrad, filter, x, y, z, skel.getSizeX(), skel.getSizeY(), skel.getSizeZ(), filter.R, false);
                        } else {
                            eigen = volumeEigens[index];
                        }
                        cost = GetVoxelCost(eigen, skelDirs[index], pruningClass);
                        if(cost < threshold) {
                            skel(x, y, z) = 0.0;
                        }
                        costVol(x, y, z) = cost;
                    }
                }
            }
        }


        #ifdef SAVE_INTERMEDIATE_RESULTS
            costVol->toMRCFile((char *)((outPath + "-Score.mrc").c_str()));
            WriteEigenResultsToVRMLFile(src, costVol, tempSkel, volumeEigens, outPath + "-Eigens.wrl", (pruningClass != PRUNING_CLASS_PRUNE_SURFACES));
            WriteEigenResultsToVRMLFile(src, costVol, tempSkel, volumeEigens, outPath + "-Eigens-inverted.wrl", true);
            WriteEigenResultsToVRMLFile(src, costVol, tempSkel, volumeEigens, outPath + "-Eigens.wrl", false);
            WriteSkeletonDirectionToVRMLFile(tempSkel, costVol, skelDirs, outPath + "-SkeletonDirections.wrl", pruningClass == PRUNING_CLASS_PRUNE_SURFACES, 0.1);
            if(pruningClass == PRUNING_CLASS_PRUNE_CURVES) {
                WriteSkeletonDirectionToVRMLFile(tempSkel, costVol, skelDirs, outPath + "-SkeletonDirections-small.wrl", false, 0.06);
            }
        #endif
    }

    void Skeletonizer::SmoothenVolume(Volume & src, double minGrayscale, double maxGrayscale, int stR) {
        ProbDistr3D mask;
        mask.R = 1;

        ProbDistr3D smoothenMask;
        smoothenMask.R = stR;
        BinomDistr(smoothenMask);

        src.pad(MAX_GAUSS_FILT_R, 0);

        Volume maskVolume(src.getSizeX(), src.getSizeY(), src.getSizeZ());
        double data;
        for(int x = 0; x < src.getSizeX(); x++) {
            for(int y = 0; y < src.getSizeY(); y++) {
                for(int z = 0; z < src.getSizeZ(); z++) {
                    data = src(x, y, z);
                    if((data >= minGrayscale) && (data <= maxGrayscale)) {
                        maskVolume(x, y, z) = 1.0;
                    } else {
                        maskVolume(x, y, z) = 0.0;
                    }
                }
            }
        }

        vector<Vec3F> volGrad = GetVolumeGradient(src);
        vector<EigenResults3D> eigens = GetEigenResults(maskVolume, volGrad, smoothenMask, stR, true);
        Volume dest (src.getSizeX(), src.getSizeY(), src.getSizeZ());
        double sourceData;

        for(int x = mask.R; x < src.getSizeX()-mask.R; x++) {
            for(int y = mask.R; y < src.getSizeY()-mask.R; y++) {
                for(int z = mask.R; z < src.getSizeZ()-mask.R; z++) {
                    sourceData = src(x, y, z);
                    if((sourceData >= minGrayscale) && (sourceData <= maxGrayscale)) {
                        GetSTBasedDistribution(mask, eigens[src.getIndex(x, y, z)]);

                        for(int xx = -mask.R; xx <= mask.R; xx++) {
                            for(int yy = -mask.R; yy <= mask.R; yy++) {
                                for(int zz = -mask.R; zz <= mask.R; zz++) {
                                    dest(x, y, z) = dest(x, y, z) + src(x+xx, y+yy, z+zz) *  mask.vals[xx+mask.R][yy+mask.R][zz+mask.R];
                                }
                            }
                        }
                        dest(x, y, z) = src(x, y, z) * 0.5 + dest(x, y, z) * 0.5;
                    } else {
                        dest(x, y, z) = src(x, y, z);
                    }
                }
            }
        }

        dest.pad(-MAX_GAUSS_FILT_R, 0);

        src = dest;
    }
    void Skeletonizer::VoxelBinarySubtract(Volume & dest, const Volume & src){
        for(int x = 0; x < dest.getSizeX(); x++) {
            for(int y = 0; y < dest.getSizeY(); y++) {
                for(int z = 0; z < dest.getSizeZ(); z++) {
                    if(src(x, y, z) > 0) {
                        dest(x, y, z) = 0;
                    }
                }
            }
        }
    }

    void Skeletonizer::VoxelSubtract(Volume & dest, const Volume & src){
        for(int x = 0; x < dest.getSizeX(); x++) {
            for(int y = 0; y < dest.getSizeY(); y++) {
                for(int z = 0; z < dest.getSizeZ(); z++) {
                    dest(x, y, z) = dest(x, y, z) - src(x, y, z);
                }
            }
        }
    }

    void Skeletonizer::VoxelOr(Volume & dest, const Volume * src){
        if(src != NULL) {
            for(int x = 0; x < dest.getSizeX(); x++) {
                for(int y = 0; y < dest.getSizeY(); y++) {
                    for(int z = 0; z < dest.getSizeZ(); z++) {
                        dest(x, y, z) = max(dest(x, y, z), (*src)(x, y, z));
                    }
                }
            }
        }
    }


    Volume Skeletonizer::FillCurveHoles(Volume & dest, const Volume & src, int maxHoleSize) {
        Volume holes(src);
        VoxelSubtract(holes, dest);
        PruneCurves(holes, maxHoleSize);

        Volume filledSkeleton(src);
        VoxelSubtract(filledSkeleton, holes);

        return filledSkeleton;
    }

    Volume Skeletonizer::FillSurfaceHoles(Volume & dest, const Volume & src, int maxHoleSize) {
        Volume holes(src);

        VoxelSubtract(holes, dest);
        PruneSurfaces(holes, maxHoleSize);

        Volume filledSkeleton(src);
        VoxelSubtract(filledSkeleton, holes);

        return filledSkeleton;
    }

    Volume * Skeletonizer::GetImmersionThinning(const Volume & src, const Volume & preserve, double lowGrayscale, double highGrayscale, double stepSize, char thinningClass) {
        Volume * thinnedVolume = new Volume(src);
        thinnedVolume->threshold2(lowGrayscale, 0, 1) ;
        double t;
        switch(thinningClass) {
            case THINNING_CLASS_SURFACE_PRESERVATION :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->surfaceSkeleton(src, t, t + stepSize) ;
                }
                break;
            case THINNING_CLASS_CURVE_PRESERVATION :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->curveSkeleton(src, t, t + stepSize, preserve);
                }
                break;
            case THINNING_CLASS_CURVE_PRESERVATION_2D :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->curveSkeleton(src, t, t + stepSize, preserve);
                }
                break;
            case THINNING_CLASS_POINT_PRESERVATION :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->pointSkeleton(src, t, t + stepSize, preserve, preserve);
                }
        }
        return thinnedVolume;
    }


    Volume * Skeletonizer::GetJuCurveSkeleton(Volume & src, Volume & preserve, double threshold, bool is3D){
        char thinningClass = is3D ? THINNING_CLASS_CURVE_PRESERVATION : THINNING_CLASS_CURVE_PRESERVATION_2D;
        return GetJuThinning(src, preserve, threshold, thinningClass);
    }

    Volume * Skeletonizer::GetJuSurfaceSkeleton(Volume & src, Volume & preserve, double threshold){
        return GetJuThinning(src, preserve, threshold, THINNING_CLASS_SURFACE_PRESERVATION);
    }

    Volume * Skeletonizer::GetJuTopologySkeleton(Volume & src, Volume & preserve, double threshold){
        return GetJuThinning(src, preserve, threshold, THINNING_CLASS_TOPOLOGY_PRESERVATION);
    }


    Volume * Skeletonizer::GetJuThinning(Volume & src, const Volume & preserve, double threshold, char thinningClass) {
        Volume * thinnedVolume = new Volume(src);
        switch(thinningClass) {
            case THINNING_CLASS_SURFACE_PRESERVATION :
                thinnedVolume->surfaceSkeletonPres(threshold, preserve);
                break;
            case THINNING_CLASS_CURVE_PRESERVATION :
                thinnedVolume->curveSkeleton(threshold, preserve);
                break;
            case THINNING_CLASS_CURVE_PRESERVATION_2D :
                thinnedVolume->curveSkeleton2D(threshold, preserve);
                break;
            case THINNING_CLASS_TOPOLOGY_PRESERVATION :
                thinnedVolume->skeleton(threshold, preserve, preserve);
        }

        return thinnedVolume;
    }

    Volume * Skeletonizer::PerformImmersionSkeletonizationAndPruning(
            Volume & sourceVol, Volume * preserved, double startGray,
            double endGray, double stepSize, int smoothingIterations,
            int smoothingR, int minCurveSize, int minSurfaceSize,
            int maxCurveHole, int maxSurfaceHole, string outPath,
            bool doPruning, double pointThreshold, double curveThreshold,
            double surfaceThreshold)
    {
        appTimeManager.PushCurrentTime();
        for(int i = 0; i < smoothingIterations; i++) {
            SmoothenVolume(sourceVol, startGray, endGray, smoothingR);
        }
        appTimeManager.PopAndDisplayTime("Smoothing : %f seconds!\n");
        vector<Vec3F> volGrad;
        vector<EigenResults3D> volumeEigens;
        sourceVol.pad(MAX_GAUSS_FILT_R, 0);

        if(preserved != NULL)
            preserved->pad(MAX_GAUSS_FILT_R, 0);

        if(doPruning) {
            volGrad = GetVolumeGradient(sourceVol);
        }

        Volume nullVol(sourceVol.getSizeX(), sourceVol.getSizeY(), sourceVol.getSizeZ());
        appTimeManager.PushCurrentTime();
        Volume surfaceVol = *GetImmersionThinning(sourceVol, *preserved, startGray, endGray, stepSize, THINNING_CLASS_SURFACE_PRESERVATION);
        appTimeManager.PopAndDisplayTime("Surface Thinning : %f seconds!\n");

        #ifdef SAVE_INTERMEDIATE_RESULTS
            surfaceVol->toMRCFile((char *)(outPath + "-S-Pre-Prune-Pre-Erode.mrc").c_str());
        #endif

        PruneSurfaces(surfaceVol, minSurfaceSize);

        appTimeManager.PushCurrentTime();
        if(doPruning) {
            #ifdef SAVE_INTERMEDIATE_RESULTS
                surfaceVol->toMRCFile((char *)(outPath + "-S-Pre-Prune.mrc").c_str());
                WriteVolumeToVRMLFile(surfaceVol, outPath + "-S-Pre-Prune.wrl");
            #endif
            appTimeManager.PushCurrentTime();
            volumeEigens = GetEigenResults(surfaceVol, volGrad, gaussFiltSrfcR, surfR, true);
            appTimeManager.PopAndDisplayTime("  Getting Eigens : %f seconds!\n");

            appTimeManager.PushCurrentTime();
            Volume prunedSurfaceVol(surfaceVol);
            appTimeManager.PopAndDisplayTime("  Getting Copy of surface : %f seconds!\n");


            appTimeManager.PushCurrentTime();
            PruneUsingStructureTensor(prunedSurfaceVol, sourceVol, preserved, volGrad, volumeEigens, gaussFiltSrfcR, surfaceThreshold, PRUNING_CLASS_PRUNE_SURFACES, outPath + "-S");
            appTimeManager.PopAndDisplayTime("  Pruning: %f seconds!\n");

            appTimeManager.PushCurrentTime();

            #ifdef SAVE_INTERMEDIATE_RESULTS
                prunedSurfaceVol->toMRCFile((char *)(outPath + "-S-Post-Prune.mrc").c_str());
            #endif

            surfaceVol = prunedSurfaceVol;
            appTimeManager.PopAndDisplayTime("  Memory Cleanup: %f seconds!\n");

        }

        PruneSurfaces(surfaceVol, minSurfaceSize);
        appTimeManager.PopAndDisplayTime("Surface Pruning  : %f seconds!\n");

        #ifdef SAVE_INTERMEDIATE_RESULTS
            surfaceVol->toMRCFile((char *)(outPath + "-S-Post-Erosion.mrc").c_str());
        #endif

        Volume cleanedSurfaceVol = *GetJuSurfaceSkeleton(surfaceVol, nullVol, 0.5);
        PruneSurfaces(cleanedSurfaceVol, minSurfaceSize);
        #ifdef SAVE_INTERMEDIATE_RESULTS
            cleanedSurfaceVol->toMRCFile((char *)(outPath + "-S-Cleaned.mrc").c_str());
        #endif

        surfaceVol = cleanedSurfaceVol;
        VoxelOr(surfaceVol, preserved);

        appTimeManager.PushCurrentTime();

        Volume curveVol = *GetImmersionThinning(sourceVol, surfaceVol, startGray, endGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
        appTimeManager.PopAndDisplayTime("Curve Thinning   : %f seconds!\n");

        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toMRCFile((char *)(outPath + "-C-Pre-Prune_Pre-Erode.mrc").c_str());
        #endif

        PruneCurves(curveVol, minCurveSize);
        VoxelBinarySubtract(curveVol, surfaceVol);

        appTimeManager.PushCurrentTime();
        if(doPruning) {
            #ifdef SAVE_INTERMEDIATE_RESULTS
                curveVol->toMRCFile((char *)(outPath + "-C-Pre-Prune.mrc").c_str());
            #endif

            volumeEigens = GetEigenResults(curveVol, volGrad, gaussFiltCrvR, curveR, true);
            Volume prunedCurveVol(curveVol);
            PruneUsingStructureTensor(prunedCurveVol, sourceVol, preserved, volGrad, volumeEigens, gaussFiltCrvR, curveThreshold, PRUNING_CLASS_PRUNE_CURVES, outPath + "-C");

            #ifdef SAVE_INTERMEDIATE_RESULTS
                prunedCurveVol->toMRCFile((char *)(outPath + "-C-Post-Prune.mrc").c_str());
            #endif

            Volume filledCurveVol = FillCurveHoles(prunedCurveVol, curveVol, maxCurveHole);
            #ifdef SAVE_INTERMEDIATE_RESULTS
                filledCurveVol->toMRCFile((char *)(outPath + "-C-Post-Fill.mrc").c_str());
            #endif

            curveVol = filledCurveVol;
        }

        VoxelOr(curveVol, &surfaceVol);
        PruneCurves(curveVol, minCurveSize);
        appTimeManager.PopAndDisplayTime("Curve Pruning    : %f seconds!\n");
        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toMRCFile((char *)(outPath + "-C-Post-Erosion.mrc").c_str());
        #endif

        Volume cleanedCurveVol = *GetJuCurveSkeleton(curveVol, surfaceVol, 0.5, true);
        PruneCurves(cleanedCurveVol, minCurveSize);
        #ifdef SAVE_INTERMEDIATE_RESULTS
            cleanedCurveVol->toMRCFile((char *)(outPath + "-C-Cleaned.mrc").c_str());
        #endif

        curveVol = cleanedCurveVol;

        VoxelOr(curveVol, &surfaceVol);
        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toMRCFile((char *)(outPath + "-SC.mrc").c_str());
            curveVol->toOFFCells2((char *)(outPath + "-SC.off").c_str());
        #endif

        sourceVol.pad(-MAX_GAUSS_FILT_R, 0);
        curveVol.pad(-MAX_GAUSS_FILT_R, 0);

        Volume * res = new Volume(curveVol);

        return res;
    }


    Volume * Skeletonizer::PerformPureJuSkeletonization(
            Volume & imageVol, string outPath, double threshold,
            int minCurveWidth, int minSurfaceWidth)
    {
        #ifdef GORGON_DEBUG
              cout<<"\033[33mDEBUG: File:   VolumeSkeletonizer.h"<<endl;
              cout<<"DEBUG: Method: VolumeSkeletonizer::PerformPureJuSkeletonization\033[0m"<<endl;
              cout<<"DEBUG: Args: Volume*, string, double, int, int\033[0m"<<endl;
              cout<<imageVol.getSize()<<endl;
        #endif
        imageVol.pad(MAX_GAUSS_FILT_R, 0);
        #ifdef GORGON_DEBUG
              cout<<"imageVol->getSize(): "<<imageVol.getSize()<<endl;
        #endif


        Volume preservedVol(imageVol.getSizeX(), imageVol.getSizeY(), imageVol.getSizeZ());

        //printf("\t\t\tUSING THRESHOLD : %f\n", threshold);
        // Skeletonizing while preserving surface features curve features and topology
        Volume surfaceVol = *GetJuSurfaceSkeleton(imageVol, preservedVol, threshold);
        #ifdef GORGON_DEBUG
              cout<<"surfaceVol->getSize(): "<<surfaceVol.getSize()<<endl;
        #endif

        PruneSurfaces(surfaceVol, minSurfaceWidth);
        VoxelOr(preservedVol, &surfaceVol);

        Volume curveVol = *Skeletonizer::GetJuCurveSkeleton(imageVol, preservedVol, threshold, true);
        #ifdef GORGON_DEBUG
              cout<<"curveVol->getSize(): "<<curveVol.getSize()<<endl;
        #endif
        Skeletonizer::PruneCurves(curveVol, minCurveWidth);
        VoxelOr(preservedVol, &curveVol);
#ifdef GORGON_DEBUG
        cout<<"preservedVol->getSize(): "<<preservedVol.getSize()<<endl;
        cout<<"curveVol->getSize(): "<<curveVol.getSize()<<endl;
#endif

        Volume * topologyVol = Skeletonizer::GetJuTopologySkeleton(imageVol, preservedVol, threshold);
#ifdef GORGON_DEBUG
        cout<<"1: topologyVol->getSize(): "<<topologyVol->getSize()<<endl;
#endif

        imageVol.pad(-MAX_GAUSS_FILT_R, 0);
        topologyVol->pad(-MAX_GAUSS_FILT_R, 0);

        #ifdef GORGON_DEBUG
              cout<<"\033[34mDEBUG: File:   VolumeSkeletonizer.h"<<endl;
              cout<<"DEBUG: Method: VolumeSkeletonizer::PerformPureJuSkeletonization\033[0m"<<endl;
              cout<<"DEBUG: Args: Volume*, string, double, int, int\033[0m"<<endl;
              cout<<"2: topologyVol->getSize(): "<<topologyVol->getSize()<<endl;
        #endif

        return topologyVol;
    }

} /* namespace GraySkeletonCPP */
