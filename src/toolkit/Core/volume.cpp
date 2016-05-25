#include "volume.h"
#include <Foundation/StringUtils.h>
#include <Core/GlobalDefinitions.h>
#include <SkeletonMaker/Skeletonizer.h>
#include <Readers/reader.h>

#include <fstream>
#include <iomanip>

using namespace SkeletonMaker;
using namespace Foundation;
using namespace GraySkeletonCPP;

namespace Core {

#ifdef GORGON_DEBUG
int Volume::id0=0;
int Volume::id1=0;
int Volume::id3=0;
#endif
//    ---------------------
    Volume * Volume::getVolume() {
        return dynamic_cast<Volume *>(this);
    }

    void Volume::setVolume(Volume *vol) {
        *this = *vol;
    }
    Volume::Volume() {}

    Volume::Volume(int x, int y, int z, float val)
    : VolumeData(x, y, z, val)
    {
#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
        cout<<"DEBUG: Method: Volume::Volume(int, int, int, float)\033[0m"<<endl;
        cout<<"Id3: "<<id3<<endl;
        id3++;
#endif

    }

    Volume::~Volume() {
    }

    float Volume::getOffset(float fValue1, float fValue2, float fValueDesired) const {
        double fDelta = fValue2 - fValue1;
        if(fDelta == 0.0) {
            return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
    }

    float Volume::getVoxelData(int x, int y, int z) const {
        if((x < 0) || (x > getSizeX()-1) || (y < 0) || (y > getSizeY()-1) || (z < 0) || (z > getSizeZ()-1)) {
            return 0.0f;
        } else {
            return (*this)(x, y, z);
        }
    }

    float Volume::getVoxelData(float x, float y, float z) const {
        int f[3] = {(int)x, (int)y, (int)z};
        int c[3] = {f[0]+1, f[1]+1, f[2]+1};
        float d[3] = {x - f[0], y - f[1], z - f[2]};

        float i1 = getVoxelData(f[0], f[1], f[2]) * (1.0 - d[2]) + getVoxelData(f[0], f[1], c[2]) * d[2];
        float i2 = getVoxelData(f[0], c[1], f[2]) * (1.0 - d[2]) + getVoxelData(f[0], c[1], c[2]) * d[2];
        float j1 = getVoxelData(c[0], f[1], f[2]) * (1.0 - d[2]) + getVoxelData(c[0], f[1], c[2]) * d[2];
        float j2 = getVoxelData(c[0], c[1], f[2]) * (1.0 - d[2]) + getVoxelData(c[0], c[1], c[2]) * d[2];

        float w1 = i1 * (1.0 - d[1]) + i2 * d[1];
        float w2 = j1 * (1.0 - d[1]) + j2 * d[1];

        return w1 * (1.0 - d[0]) + w2 * d[0];
    }

    int Volume::getHashKey(int x, int y, int z, int edge, int iScale) const {

        x += a2iEdgeHash[edge][1]*iScale;
        y += a2iEdgeHash[edge][2]*iScale;
        z += a2iEdgeHash[edge][3]*iScale;

        edge = a2iEdgeHash[edge][0];
        return x * getSizeY() * getSizeZ() * 3 + y * getSizeZ() * 3 + z * 3 + edge;
    }

    int Volume::getNonZeroVoxelCount() {
        int count = 0;
        for(int x = 0; x < getSizeX(); x++) {
            for(int y = 0; y < getSizeY(); y++) {
                for(int z = 0; z < getSizeZ(); z++) {
                    if((*this)(x, y, z) > 0.0) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    string Volume::str() {
        ostringstream out;
        out<<*this;

        return out.str();
    }

    void Volume::print() {
        for(int x = 0; x < getSizeX(); x++) {
            printf("{ ");
            for(int y = 0; y < getSizeY(); y++) {
                printf("{ ");
                for(int z = 0; z < getSizeZ(); z++) {
                    printf("%f, ", (*this)(x, y, z));
                }
                printf("} ");
            }
            printf("} ");
        }
        printf("\n");
    }

    void Volume::subtract(const Volume & vol) {
        int i, j, k;
        for(i = 0; i < getSizeX(); i++)
            for(j = 0; j < getSizeY(); j++)
                for(k = 0; k < getSizeZ(); k++) {
                    if((*this)(i, j, k) > 0) {
                        if(vol(i, j, k) > 0) {
                            (*this)(i, j, k) = 0.0;
                        }
                    }
                }

    }

    void Volume::applyMask(const Volume & mask, double maskValue, bool keepMaskValue) {
        for(int x = 0; x < mask.getSizeX(); x++) {
            for(int y = 0; y < mask.getSizeY(); y++) {
                for(int z = 0; z < mask.getSizeZ(); z++) {
                    if( ( (mask(x, y, z) == maskValue) && !keepMaskValue) || ( (mask(x, y, z) != maskValue) && keepMaskValue)) {
                        (*this)(x, y, z) = 0;
                    }
                }
            }
        }
    }

    double Volume::getMin() const {
        return *(min_element(data.begin(), data.end()));
    }

    double Volume::getMax() const {
        return *(max_element(data.begin(), data.end()));
    }

    double Volume::getMaxValuePosition(int& maxX, int& maxY, int& maxZ) {
        double maxVal = (*this)(0, 0, 0);
        maxX = 0;
        maxY = 0;
        maxZ = 0;
        double data;

        for(int x = 0; x < getSizeX(); x++) {
            for(int y = 0; y < getSizeY(); y++) {
                for(int z = 0; z < getSizeZ(); z++) {
                    data = (*this)(x, y, z);
                    if(data > maxVal) {
                        maxVal = data;
                        maxX = x;
                        maxY = y;
                        maxZ = z;
                    }
                }
            }
        }
        return maxVal;
    }

    int Volume::getNumNeighbor6(int ox, int oy, int oz) {
        Vec3I o(ox, oy, oz);

        int rvalue = 0;
        for(int i = 0; i < 6; i++) {
            Vec3I p = o + vneighbor6[i];

            if((*this)(p) >= 0) {
                rvalue++;
            }
        }

        return rvalue;
    }

    int Volume::isInternal2(int ox, int oy, int oz) {
        // assuming it's -1/0 volume
        int i, j, k;

        for(i = -1; i < 2; i++)
            for(j = -1; j < 2; j++)
                for(k = -1; k < 2; k++) {
                    if((*this)(ox + i, oy + j, oz + k) < 0) {
                        return 0;
                    }
                }

        return 1;
    }

    int Volume::hasCell(int ox, int oy, int oz) {
        for(int i = 0; i < 2; i++)
            for(int j = 0; j < 2; j++)
                for(int k = 0; k < 2; k++) {
                    if((*this)(ox + i, oy + j, oz + k) < 0) {
                        return 0;
                    }
                }
        return 1;
    }

    Volume Volume::markCellFace() {
        Volume fvol(getSizeX(), getSizeY(), getSizeZ());

        for(int i = 0; i < getSizeX(); i++)
            for(int j = 0; j < getSizeY(); j++)
                for(int k = 0; k < getSizeZ(); k++) {
                    if((*this)(i, j, k) >= 0) {
                        if(hasCell(i, j, k)) {
                            for(int m = 0; m < 6; m++) {
                                int nx = i + neighbor6[m][0];
                                int ny = j + neighbor6[m][1];
                                int nz = k + neighbor6[m][2];
                                if(!hasCell(nx, ny, nz)) {
                                    fvol(i, j, k) = (double) (1 << m);
                                    break;
                                }
                            }
                        }
                    }
                }

        return fvol;
    }

    int Volume::getN6(vector<Vec3I> & n6, int x, int y, int z) const {
        int n6Count = 0;
        n6.resize(6);
        for(int i = 0; i < 6; i++) {
            n6[n6Count][0] = x + VOLUME_NEIGHBORS_6[i][0];
            n6[n6Count][1] = y + VOLUME_NEIGHBORS_6[i][1];
            n6[n6Count][2] = z + VOLUME_NEIGHBORS_6[i][2];
            if((*this)(n6[n6Count]) > 0) {
                n6Count++;
            }
        }
        return n6Count;
    }

    int Volume::getN6Count(int x, int y, int z) const {
        vector<Vec3I> n6;
        return getN6(n6, x, y, z);
    }

    int Volume::hasCompleteSheet(int ox, int oy, int oz) {
        // Returns 1 if it lies in the middle of a sheet
        int temp = countIntEuler(ox, oy, oz);
        if(temp > 0) {
            return 1;
        }
        else {
            return 0;
        }
    }

    int Volume::hasCompleteHelix(int ox, int oy, int oz) {
        // Returns 1 if it has a complete helix
        int i;
        int c1 = 0;
        int nx, ny, nz;
        int j;

        for(i = 0; i < 6; i++) {
            nx = ox + neighbor6[i][0];
            ny = oy + neighbor6[i][1];
            nz = oz + neighbor6[i][2];
            if((*this)(nx, ny, nz) >= 0) {
                c1++;
                j = i;
            }
        }

        if(c1 > 1) // || c1 == 0 )
        {
            return 1;
        }
        return 0;
    }

    int Volume::isHelixEnd(int ox, int oy, int oz) {
        int c1 = 0, c2 = 0;
        int nx, ny, nz;

        for(int i = 0; i < 6; i++) {
            nx = ox + neighbor6[i][0];
            ny = oy + neighbor6[i][1];
            nz = oz + neighbor6[i][2];

            double val = (*this)(nx, ny, nz);

            if(val >= 0) {
                c1++;
                if(getNumNeighbor6(nx, ny, nz) < 6) {
                    c2++;
                }
            }
        }

        if(c1 == 1 && c2 == 1) {
            return 1;
        }

        return 0;
    }

    int Volume::isFeatureFace(int ox, int oy, int oz) {
        int i, j;
        int nx, ny, nz;

        int faces = 12;
        for(i = 0; i < 12; i++) {
            int ct = 0;
            for(j = 0; j < 4; j++) {
                nx = ox + sheetNeighbor[i][j][0];
                ny = oy + sheetNeighbor[i][j][1];
                nz = oz + sheetNeighbor[i][j][2];

                if((*this)(nx, ny, nz) < 0) {
                    ct = -1;
                    break;
                }
                else if(getNumNeighbor6(nx, ny, nz) == 6) {
                    ct = -1;
                    break;
                }
            }
            if(ct == -1 || ct >= 1) {
                faces--;
            }
        }

        if(faces > 0) {
            return 1;
        }
        return 0;

    }

    int Volume::isSheetEnd(int ox, int oy, int oz) {
        return ( (hasCompleteSheet(ox, oy, oz) == 0) && isFeatureFace(ox, oy, oz));
    }

    bool Volume::isSimple(int ox, int oy, int oz) {
        /* Test if this is a simple voxel */
        // int flag = 0 ;
        double vox[3][3][3];

        for(int i = -1; i < 2; i++)
            for(int j = -1; j < 2; j++)
                for(int k = -1; k < 2; k++) {
                    double tval = (*this)(ox + i, oy + j, oz + k);
                    vox[i + 1][j + 1][k + 1] = tval;
                }

        if(countInt(vox) == 1 && countNeg(vox) == 1) {
            return true;
        }
        else {
            return false;
        }
}

int Volume::isPiercable(int ox, int oy, int oz) {
    /* Test if this is a simple voxel */
    // int flag = 0 ;
    double vox[3][3][3];

    int i, j, k;
    for(i = -1; i < 2; i++)
        for(j = -1; j < 2; j++)
            for(k = -1; k < 2; k++) {
                double tval = (*this)(ox + i, oy + j, oz + k);
                vox[i + 1][j + 1][k + 1] = tval;
            }

    if(countInt(vox) == 1 && countNeg(vox) != 1) {
        return 1;
    }
    else {
        return 0;
    }
}

int Volume::getNumPotComplex(int ox, int oy, int oz) {
    int i;
    double val = (*this)(ox, oy, oz);

    int rvalue = 0, nx, ny, nz;
    (*this)(ox, oy, oz) = -val;

    for(i = 0; i < 6; i++) {
        nx = ox + neighbor6[i][0];
        ny = oy + neighbor6[i][1];
        nz = oz + neighbor6[i][2];

        if((*this)(nx, ny, nz) >= 0) {
            int num = getNumNeighbor6(nx, ny, nz);
            if(num > rvalue) {
                rvalue = num;
            }
        }
    }

    (*this)(ox, oy, oz) = val;

    return rvalue + getNumNeighbor6(ox, oy, oz) * 10;
}

int Volume::getNumPotComplex2(int ox, int oy, int oz) {
    return getNumPotComplex(ox, oy, oz);
}

int Volume::components(int vox[3][3][3], const vector<Vec3I> neighbors) {
    int tot = 0;
    queue<Vec3I> q;
    int vis[3][3][3]={0};

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            for(int k = 0; k < 3; k++) {
                if(vox[i][j][k]) {
                    if(tot == 0) {
                        q.push(Vec3I(i, j, k));
                        vis[i][j][k] = 1;
                    }
                    tot++;
                }
            }
    if(tot == 0) {
        return 0;
    }
    // printf("total: %d\n", tot) ;

    int ct = 1;

    while(!q.empty()) {
        Vec3I xyz = q.front();
        q.pop();

        for(int i = 0; i < neighbors.size(); i++) {
            Vec3I n = xyz + neighbors[i];

            int nx = n[0];
            int ny = n[1];
            int nz = n[2];

            if(n > Vec3I(-1,-1,-1) && n < Vec3I(3,3,3)) {
                if(vox[nx][ny][nz] && !vis[nx][ny][nz]) {
                    q.push(n);
                    vis[nx][ny][nz] = 1;
                    ct++;
                }
            }
        }
    }

    if(ct == tot) {
        return 1;
    }
    else {
        return 2;
    }

}

int Volume::components6(int vox[3][3][3]) {
    return components(vox, vneighbor6);
}
int Volume::components26(int vox[3][3][3]) {
    return components(vox, vneighbor26);
}

int Volume::countNeg(double vox[3][3][3]) {
    int tvox[3][3][3]={0};

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            for(int k = 0; k < 3; k++) {
                if(vox[i][j][k] < 0)
                    tvox[i][j][k] = 1;
            }

    return components26(tvox);
}

int Volume::countInt(double vox[3][3][3]) {
    int tvox[3][3][3]={0};

    for(int i = 0; i < 6; i++) {
        int nx = 1 + neighbor6[i][0];
        int ny = 1 + neighbor6[i][1];
        int nz = 1 + neighbor6[i][2];
        if(vox[nx][ny][nz] >= 0) {
            tvox[nx][ny][nz] = 1;
            for(int j = 0; j < 4; j++) {
                int nnx = neighbor64[i][j][0] + nx;
                int nny = neighbor64[i][j][1] + ny;
                int nnz = neighbor64[i][j][2] + nz;
                if(vox[nnx][nny][nnz] >= 0) {
                    tvox[nnx][nny][nnz] = 1;
                }
            }
        }
    }

    return components6(tvox);
}

int Volume::countIntEuler(int ox, int oy, int oz) {
    int nv = 0, ne = 0, nc = 0;

    int i, j, k;
    int tvox[3][3][3]={0};
    double vox[3][3][3];

    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            for(k = 0; k < 3; k++) {
                vox[i][j][k] = (*this)(ox - 1 + i, oy - 1 + j, oz - 1 + k);
            }

    for(i = 0; i < 6; i++) {
        int nx = 1 + neighbor6[i][0];
        int ny = 1 + neighbor6[i][1];
        int nz = 1 + neighbor6[i][2];
        if(vox[nx][ny][nz] >= 0) {
            tvox[nx][ny][nz] = 1;

            nv++;

            for(j = 0; j < 4; j++) {
                int nnx = neighbor64[i][j][0] + nx;
                int nny = neighbor64[i][j][1] + ny;
                int nnz = neighbor64[i][j][2] + nz;
                if(vox[nnx][nny][nnz] >= 0) {
                    if(tvox[nnx][nny][nnz] == 0) {
                        tvox[nnx][nny][nnz] = 1;
                        nv++;
                    }

                    ne++;
                }
            }
        }
    }

    nc = components6(tvox);

    return (nc - (nv - ne));
}

/* Thin the current volume while preserving voxels with values > highthr or <= lowthr in grayvol
 *  Assuming the current volume has already been thresholded to 0/1
 */
void Volume::curveSkeleton(const Volume & grayvol, float lowthr, float highthr,
                           const Volume & src)
{
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n");
#endif
    threshold(0.5f, -1, 0);

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    List* queue2 = new List();
    List* queue3 = new List();
    List* queue4 = new List();
    PriorityQueue < gridPoint > queue(MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    double v = grayvol(i, j, k);
                    if(src(i, j, k) > 0
                       || v <= lowthr || v > highthr) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        for(int m = 0; m < 6; m++) {
                            if((*this)(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                // (*this)( i, j, k, 1 ) ;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );
    printf("Start erosion to %d...\n", wid);
#endif

    // Perform erosion
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        /*
         We first need to assign curwid + 1 to every node in this layer
         */
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            if((*this)(ox, oy, oz) == curwid) {
                ele = queue2->remove();
            }
            else {
                (*this)(ox, oy, oz) = curwid;
                ele = queue2->getNext();
            }
        }
        queue4->reset();
        ele = queue4->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            queue2->prepend(ox, oy, oz);
            ele = queue4->remove();
        }

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex2(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy, oz) != score) {
                continue;
            }

            if(isHelixEnd(ox, oy, oz) || !isSimple(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue4->prepend(ox, oy, oz);
                numComplex++;
            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;

                /* Adding ends */
                // Move its neighboring unvisited node to queue2
                for(int m = 0; m < 6; m++) {
                    int nx = ox + neighbor6[m][0];
                    int ny = oy + neighbor6[m][1];
                    int nz = oz + neighbor6[m][2];
                    if((*this)(nx, ny, nz) == 0) {
                        // (*this)( nx, ny, nz, curwid + 1 ) ;
                        queue2->prepend(nx, ny, nz);
                    }
                }

            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue
            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex2(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            break;
        }
    }

    // Remove all internal voxels (contained in manifold surfaces)
    queue2->reset();
    queue4->reset();
    ele = queue4->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;

        if(isPiercable(ox, oy, oz) == 1) // hasCompleteSheet( ox, oy, oz ) == 1 ) //
        {
            queue2->prepend(ox, oy, oz);
            //	(*this)( ox, oy, oz, -1 ) ;
        }
        ele = queue4->remove();
    }

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) == 0 && isPiercable(i, j, k)) //hasCompleteSheet(i,j,k) == 1) //
                {
                    queue2->prepend(i, j, k);
                }
            }
    queue2->reset();
    ele = queue2->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;
        (*this)(ox, oy, oz) = -1;
        ele = queue2->remove();
    }

    // Finally, clean up
    delete scrvol;

    delete queue2;
    delete queue3;
    delete queue4;
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);
}

// Compute curve skeleton
void Volume::curveSkeleton(float thr, const Volume & src) {
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n");
#endif
    threshold(thr, -1, 0);

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    List* queue2 = new List();
    List* queue3 = new List();
    List* queue4 = new List();
    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    if(src(i, j, k) > 0) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        for(int m = 0; m < 6; m++) {
                            if((*this)(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                // (*this)( i, j, k, 1 ) ;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );
    printf("Start erosion to %d...\n", wid);
#endif

    // Perform erosion
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        /*
         We first need to assign curwid + 1 to every node in this layer
         */
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            if((*this)(ox, oy, oz) == curwid) {
                ele = queue2->remove();
            }
            else {
                (*this)(ox, oy, oz) = curwid;
                ele = queue2->getNext();
            }
        }
        queue4->reset();
        ele = queue4->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            queue2->prepend(ox, oy, oz);
            ele = queue4->remove();
        }

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex2(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy, oz) != score) {
                continue;
            }

            if(isHelixEnd(ox, oy, oz) || !isSimple(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue4->prepend(ox, oy, oz);
                numComplex++;
            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;
            }
            /* Adding ends */

            // Move its neighboring unvisited node to queue2
            for(int m = 0; m < 6; m++) {
                int nx = ox + neighbor6[m][0];
                int ny = oy + neighbor6[m][1];
                int nz = oz + neighbor6[m][2];
                if((*this)(nx, ny, nz) == 0) {
                    // (*this)( nx, ny, nz, curwid + 1 ) ;
                    queue2->prepend(nx, ny, nz);
                }
            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue
            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex2(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            break;
        }
    }

    // Finally, clean up
    delete scrvol;

    delete queue2;
    delete queue3;
    delete queue4;
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);
}

// Compute curve skeleton in 2D
void Volume::curveSkeleton2D(float thr, const Volume & src) {
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n");
#endif
    threshold(thr, -1, 0);

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    List* queue2 = new List();
    List* queue3 = new List();
    List* queue4 = new List();
    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    if(src(i, j, k) > 0) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        for(int m = 0; m < 4; m++) {
                            if((*this)(i + neighbor4[m][0],
                                       j + neighbor4[m][1], k)
                               < 0) {
                                // (*this)( i, j, k, 1 ) ;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );
    printf("Start erosion to %d...\n", wid);
#endif

    // Perform erosion
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        /*
         We first need to assign curwid + 1 to every node in this layer
         */
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            if((*this)(ox, oy, oz) == curwid) {
                ele = queue2->remove();
            }
            else {
                (*this)(ox, oy, oz) = curwid;
                ele = queue2->getNext();
            }
        }
        queue4->reset();
        ele = queue4->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            queue2->prepend(ox, oy, oz);
            ele = queue4->remove();
        }

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex2(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy, oz) != score) {
                continue;
            }

            if(isHelixEnd(ox, oy, oz) || !isSimple(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue4->prepend(ox, oy, oz);
                numComplex++;
            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;
            }
            /* Adding ends */

            // Move its neighboring unvisited node to queue2
            for(int m = 0; m < 4; m++) {
                int nx = ox + neighbor4[m][0];
                int ny = oy + neighbor4[m][1];
                int nz = oz;
                if((*this)(nx, ny, nz) == 0) {
                    // (*this)( nx, ny, nz, curwid + 1 ) ;
                    queue2->prepend(nx, ny, nz);
                }
            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue
            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex2(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            break;
        }
    }

    // Finally, clean up
    delete scrvol;

    delete queue2;
    delete queue3;
    delete queue4;
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);
}

/* Thin the current volume while preserving voxels with values > highthr or <= lowthr in grayvol
 *  Assuming the current volume has already been thresholded to 0/1
 */
void Volume::pointSkeleton(const Volume & grayvol, float lowthr, float highthr,
                           const Volume & src, const Volume & hvol)
{
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n");
#endif
    threshold(0.5f, -1, 0);

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    List* queue2 = new List();
    List* queue3 = new List();
    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    float v = (float)grayvol(i, j, k);
                    if(v <= lowthr || v > highthr
                       || src(i, j, k) > 0
                       || hvol(i, j, k) > 0) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        for(int m = 0; m < 6; m++) {
                            if((*this)(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                (*this)(i, j, k) = 1;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );
#endif

    // Perform erosion
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Start erosion to %d...\n", wid);
#endif
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex2(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy,
                                                          oz)
                                                  != score) {
                continue;
            }

            /* Added for debugging */
            // Check simple
            if(!isSimple(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue2->prepend(ox, oy, oz);
                numComplex++;
            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;
                /* Adding ends */

                // Move its neighboring unvisited node to queue2
                for(int m = 0; m < 6; m++) {
                    int nx = ox + neighbor6[m][0];
                    int ny = oy + neighbor6[m][1];
                    int nz = oz + neighbor6[m][2];
                    if((*this)(nx, ny, nz) == 0) {
                        (*this)(nx, ny, nz) = curwid + 1;
                        queue2->prepend(nx, ny, nz);
                    }
                }

            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue

            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex2(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }
#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            break;
        }
    }

    // Remove all internal voxels (contained in manifold curves)
    queue2->reset();
    ele = queue2->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;

        if(hasCompleteHelix(ox, oy, oz) == 1) {
            ele = queue2->getNext();
        }
        else {
            ele = queue2->remove();
        }
    }

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) == 0 && hasCompleteHelix(i, j, k) == 1) {
                    queue2->prepend(i, j, k);
                }
            }
    queue2->reset();
    ele = queue2->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;
        (*this)(ox, oy, oz) = -1;
        ele = queue2->remove();
    }

    // Finally, clean up
    delete scrvol;

    delete queue2;
    delete queue3;
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);
}

// Compute minimal skeleton
void Volume::skeleton(float thr, const Volume & src, const Volume & hvol) {
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n");
#endif
    threshold(thr, -1, 0);

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    List* queue2 = new List();
    List* queue3 = new List();
    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    if(src(i, j, k) > 0 || hvol(i, j, k)
                            > 0) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        for(int m = 0; m < 6; m++) {
                            if((*this)(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                (*this)(i, j, k) = 1;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );

    // Perform erosion
    printf("Start erosion to %d...\n", wid);
#endif
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex2(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy, oz) != score) {
                continue;
            }

            /* Added for debugging */
            // Check simple
            if(!isSimple(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue2->prepend(ox, oy, oz);
                numComplex++;
            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;
            }
            /* Adding ends */

            // Move its neighboring unvisited node to queue2
            for(int m = 0; m < 6; m++) {
                int nx = ox + neighbor6[m][0];
                int ny = oy + neighbor6[m][1];
                int nz = oz + neighbor6[m][2];
                if((*this)(nx, ny, nz) == 0) {
                    (*this)(nx, ny, nz) = curwid + 1;
                    queue2->prepend(nx, ny, nz);
                }
            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue

            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex2(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            delete queue2;
            break;
        }
    }

    // Finally, clean up
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);
    delete scrvol;

    delete queue3;
}

void Volume::erodeHelix(int disthr) {
    int i, j, k;
    // First, threshold the volume
    //printf("Thresholding the volume to -1/0...\n") ;
    threshold(0.1f, -1, 0);

    /* Debug: remove faces */
    //Volume* facevol = markFaceEdge() ;
    /* End debugging */

    // Next, initialize the linked queue
    //printf("Initializing queue...\n") ;
    Volume * fvol = new Volume(getSizeX(), getSizeY(), getSizeZ());
    List* queue2 = new List();
    List* queue3 = new List();
    List** queues = new List*[10000];

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    if(!hasCompleteHelix(i, j, k))
                    // if ( ! hasCompleteHelix( i, j, k, facevol ) )
                            {
                        queue2->prepend(i, j, k);
                        (*fvol)(i, j, k) = -1;
                    }
                }
            }
    //printf("Total %d nodes\n", queue2->size() ) ;

    // Start erosion
    QueueNode* ele;
    int dis = -1;
    while(queue2->size() > 0) {
        // First, set distance
        dis--;
        queues[-dis] = new List();
        //printf("Distance transform to %d...", dis) ;
        queue2->reset();
        while( (ele = queue2->getNext()) != NULL) {
            (*this)(ele->x, ele->y, ele->z) = dis;
            queues[-dis]->prepend(ele->x, ele->y, ele->z);
        }
        //printf("%d nodes\n", queues[-dis]->size()) ;

        // Next, find next layer
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            for(int m = 0; m < 6; m++) {
                int nx = ele->x + neighbor6[m][0];
                int ny = ele->y + neighbor6[m][1];
                int nz = ele->z + neighbor6[m][2];
                if((*this)(nx, ny, nz) == 0) {
                    (*fvol)(nx, ny, nz) = dis;

                    if(!hasCompleteHelix(nx, ny, nz))
                    // if ( ! hasCompleteHelix( nx, ny, nz, facevol ) )
                            {
                        (*this)(nx, ny, nz) = 1;
                        queue3->prepend(nx, ny, nz);
                    }
                }
            }

            ele = queue2->remove();
        }

        // Next, swap queues
        List* temp = queue2;
        queue2 = queue3;
        queue3 = temp;
    }

    // Deal with closed rings
    dis--;
    queues[-dis] = new List();
#ifdef VERBOSE
    printf("Detecting closed rings %d...", dis);
#endif
    int ftot = 0;
    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) == 0) {
                    (*this)(i, j, k) = dis;
                    queues[-dis]->prepend(i, j, k);
                    ftot++;
                }
            }
#ifdef VERBOSE
    printf("%d nodes\n", ftot);
#endif

    // Dilate back
    // Starting from nodes with distance - 2 - disthr
    if(disthr + 2 > -dis) {
        disthr = -dis - 2;
    }
    int d;

    for(d = -dis; d > disthr + 1; d--) {
        queues[d]->reset();
        while( (ele = queues[d]->getNext()) != NULL) {
            (*this)(ele->x, ele->y, ele->z) = d;
        }
    }

    for(d = disthr + 1; d >= 2; d--) {
        //delete queue3 ;
        //queue3 = new GridQueue2( ) ;
        queues[d]->reset();
        ele = queues[d]->getNext();
        while(ele != NULL) {
            int dilatable = 0;
            for(int m = 0; m < 6; m++) {
                int nx = ele->x + neighbor6[m][0];
                int ny = ele->y + neighbor6[m][1];
                int nz = ele->z + neighbor6[m][2];
                if((*this)(nx, ny, nz) == d + 1) {
                    dilatable = 1;
                    break;
                }
            }

            if(!dilatable) {
                (*this)(ele->x, ele->y, ele->z) = -d + 1;
                if(d > 2) {
                    queues[d - 1]->prepend(ele->x, ele->y, ele->z);
                }
                ele = queues[d]->remove();
            }
            else {
                (*this)(ele->x, ele->y, ele->z) = d;
                ele = queues[d]->getNext();
            }

        }

        /* Debug: extract minimal set */
        while(1) {
            int num = 0;
            queues[d]->reset();
            ele = queues[d]->getNext();
            while(ele != NULL) {
                int critical = 0;
                (*this)(ele->x, ele->y, ele->z) = -1;

                for(i = -1; i < 2; i++) {
                    for(j = -1; j < 2; j++) {
                        for(k = -1; k < 2; k++) {
                            if(i != 0 && j != 0 && k != 0) {
                                continue;
                            }
                            int nx = ele->x + i;
                            int ny = ele->y + j;
                            int nz = ele->z + k;
                            if((*this)(nx, ny, nz) == d + 1 && !hasCompleteHelix(
                                       nx, ny, nz)) //, facevol ) )
                                       {
                                critical = 1;
                                break;
                            }
                        }
                        if(critical) {
                            break;
                        }
                    }
                    if(critical) {
                        break;
                    }
                }

                if(critical) {
                    (*this)(ele->x, ele->y, ele->z) = d;
                    ele = queues[d]->getNext();
                }
                else {
                    (*this)(ele->x, ele->y, ele->z) = -d + 1;
                    if(d > 2) {
                        queues[d - 1]->prepend(ele->x, ele->y, ele->z);
                    }
                    ele = queues[d]->remove();
                    num++;
                }

            }

#ifdef VERBOSE
            printf("Non-minimal: %d\n", num);
#endif

            if(num == 0) {
                break;
            }
        }

        /* End of debugging */
    }

    // Finally, threshold the volume
#ifdef VERBOSE
    //printf("Thresholding the volume to 0/1...\n") ;
#endif
    //threshold( -1, 1, 0, 0 ) ;
    threshold(0, 0, 1);
    delete fvol;
    delete queue2;
    delete queue3;
    for(d = -dis; d >= 2; d--) {
        delete queues[d];
    }
    delete[] queues;

}

int Volume::erodeSheet(int disthr) {
    int i, j, k;
    // First, threshold the volume
    //printf("Thresholding the volume to -1/0...\n") ;
    threshold(0.1f, -1, 0);

    /* Debug: remove cells */
    Volume facevol = markCellFace();
    /* End debugging */

    // Next, initialize the linked queue
    //printf("Initializing queue...\n") ;
    Volume * fvol = new Volume(getSizeX(), getSizeY(), getSizeZ());
    List* queue2 = new List();
    List* queue3 = new List();
    List** queues = new List*[10000];

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    if(!hasCompleteSheet(i, j, k)) {
                        queue2->prepend(i, j, k);
                        (*fvol)(i, j, k) = -1;
                    }
                }
            }
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );
#endif

    // Start erosion
    QueueNode* ele;
    int dis = -1;
    while(queue2->size() > 0) {
        // First, set distance
        dis--;
        queues[-dis] = new List();
        //printf("Distance transform to %d...", dis) ;
        queue2->reset();
        while( (ele = queue2->getNext()) != NULL) {
            (*this)(ele->x, ele->y, ele->z) = dis;
            queues[-dis]->prepend(ele->x, ele->y, ele->z);
        }
        //printf("%d nodes\n", queues[-dis]->size()) ;

        // Next, find next layer
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            // for ( int m = 0 ; m < 6 ; m ++ )
            for(int mx = -1; mx < 2; mx++)
                for(int my = -1; my < 2; my++)
                    for(int mz = -1; mz < 2; mz++) {
                        if(mx != 0 && my != 0 && mz != 0) {
                            continue;
                        }
                        int nx = ele->x + mx;
                        int ny = ele->y + my;
                        int nz = ele->z + mz;

                        if((*this)(nx, ny, nz) == 0) {
                            (*fvol)(nx, ny, nz) = dis;

                            if(!hasCompleteSheet(nx, ny, nz)) {
                                (*this)(nx, ny, nz) = 1;
                                queue3->prepend(nx, ny, nz);
                            }
                        }
                    }

            ele = queue2->remove();
        }

        // Next, swap queues
        List* temp = queue2;
        queue2 = queue3;
        queue3 = temp;
    }

    /* Deal with closed rings */

    dis--;
    queues[-dis] = new List();
#ifdef VERBOSE
    printf("Detecting closed rings %d...", dis);
#endif
    int ftot = 0;
    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) == 0) {
                    (*this)(i, j, k) = dis;
                    queues[-dis]->prepend(i, j, k);

                    ftot++;
                }
            }
#ifdef VERBOSE
    printf("%d nodes\n", ftot);
#endif
    // Dilate back
    // Starting from nodes with distance - 2 - disthr
    int d;
    if(disthr + 2 > -dis) {
        disthr = -dis - 2;

    }
    for(d = -dis; d > disthr + 1; d--) {
        queues[d]->reset();
        while( (ele = queues[d]->getNext()) != NULL) {
            (*this)(ele->x, ele->y, ele->z) = d;
        }
    }

    for(d = disthr + 1; d >= 2; d--) {

        //delete queue3 ;
        //queue3 = new GridQueue2( ) ;
        queues[d]->reset();
        ele = queues[d]->getNext();
        while(ele != NULL) {
            int dilatable = 0;
            for(i = 0; i < 12; i++) {
                int flag = 1, flag2 = 0;
                for(j = 0; j < 4; j++) {
                    int nx = ele->x + sheetNeighbor[i][j][0];
                    int ny = ele->y + sheetNeighbor[i][j][1];
                    int nz = ele->z + sheetNeighbor[i][j][2];

                    double val = (*this)(nx, ny, nz);

                    if(val > -d && val < 0) {
                        flag = 0;
                        break;
                    }
                    if(val == d + 1) {
                        flag2++;
                    }
                }

                if(flag && flag2) {
                    dilatable = 1;
                    break;
                }
            }

            if(!dilatable) {
                (*this)(ele->x, ele->y, ele->z) = -d + 1;
                if(d > 2) {
                    queues[d - 1]->prepend(ele->x, ele->y, ele->z);
                }
                ele = queues[d]->remove();
            }
            else {
                (*this)(ele->x, ele->y, ele->z) = d;
                ele = queues[d]->getNext();
            }
        }

        /* Debug: extract minimal set */
        while(1) {
            int num = 0;
            queues[d]->reset();
            ele = queues[d]->getNext();
            while(ele != NULL) {
                int critical = 0;
                (*this)(ele->x, ele->y, ele->z) = -1;

                for(i = -1; i < 2; i++) {
                    for(j = -1; j < 2; j++) {
                        for(k = -1; k < 2; k++) {
                            if(i != 0 && j != 0 && k != 0) {
                                continue;
                            }
                            int nx = ele->x + i;
                            int ny = ele->y + j;
                            int nz = ele->z + k;
                            // if ( (*this)(nx,ny,nz) == d + 1 && !hasCompleteSheet( nx,ny,nz, facevol ) )
                            if((*this)(nx, ny, nz) == d + 1 && !hasCompleteSheet(
                                       nx, ny, nz)) {
                                critical = 1;
                                break;
                            }
                        }
                        if(critical) {
                            break;
                        }
                    }
                    if(critical) {
                        break;
                    }
                }

                if(critical) {
                    (*this)(ele->x, ele->y, ele->z) = d;
                    ele = queues[d]->getNext();
                }
                else {
                    (*this)(ele->x, ele->y, ele->z) = -d + 1;
                    if(d > 2) {
                        queues[d - 1]->prepend(ele->x, ele->y, ele->z);
                    }
                    ele = queues[d]->remove();
                    num++;
                }

            }
#ifdef VERBOSE
            printf("Non-minimal: %d\n", num);
#endif

            if(num == 0) {
                break;
            }
        }
        /* End of debugging */
    }

    // Finally, threshold the volume
#ifdef VERBOSE
    //printf("Thresholding the volume to 0/1...\n") ;
#endif
    threshold(0, 0, 1);

    delete fvol;
    delete queue2;
    delete queue3;
    for(d = -dis; d >= 2; d--) {
        delete queues[d];
    }
    delete[] queues;

    return -dis - 1;
}

/* Thin the current volume while preserving voxels with values > highthr or <= lowthr in grayvol
 *  Assuming the current volume has already been thresholded to 0/1
 */
void Volume::surfaceSkeleton(const Volume & grayvol, float lowthr, float highthr) {
    int i, j, k;
    threshold(0.5f, -MAX_ERODE, 0);

    List* queue2 = new List();
    List* queue3 = new List();
    List* queue4 = new List();

    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);
    int ct = 0;

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    float v = (float)grayvol(i, j, k);
                    if(v > highthr || v <= lowthr) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        ct++;

                        for(int m = 0; m < 6; m++) {
                            if((*this)(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }

    // Perform erosion
    int wid = MAX_ERODE;
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            if((*this)(ox, oy, oz) == curwid) {
                ele = queue2->remove();
            }
            else {
                (*this)(ox, oy, oz) = curwid;
                ele = queue2->getNext();
            }
        }
        queue4->reset();
        ele = queue4->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            queue2->prepend(ox, oy, oz);
            ele = queue4->remove();
        }

        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        int nowComplex = 0;

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;


            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy, oz) != score) {
                continue;
            }

            if( (!isSimple(ox, oy, oz)) || isSheetEnd(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue4->prepend(ox, oy, oz);
                numComplex++;

                nowComplex = 1;
            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;

                for(int m = 0; m < 6; m++) {
                    int nx = ox + neighbor6[m][0];
                    int ny = oy + neighbor6[m][1];
                    int nz = oz + neighbor6[m][2];
                    if((*this)(nx, ny, nz) == 0) {
                        // (*this)( nx, ny, nz, curwid + 1 ) ;
                        queue2->prepend(nx, ny, nz);
                    }
                }

                if(nowComplex) {

                    // printf("Error: %d\n", score);
                }
            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue

            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

        if(numSimple == 0) {
            if(queue2->size() > 0) {
                printf(
                        "*************************wierd**********************\n");
            }
            break;
        }
    }

    // Remove all internal voxels (contained in cells)

    queue4->reset();
    ele = queue4->getNext();
    while(ele != NULL) {
        ele = queue4->remove();
    }

    queue2->reset();
    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) == 0 && isInternal2(i, j, k) == 1) {
                    queue2->prepend(i, j, k);
                }
            }
    queue2->reset();
    ele = queue2->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;
        (*this)(ox, oy, oz) = -1;
        ele = queue2->remove();
    }

    // Finally, clean up
    delete scrvol;

    delete queue2;
    delete queue3;
    delete queue4;
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);

}

void Volume::surfaceSkeletonPres(float thr, const Volume & preserve) {
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -MAX_ERODE/0...\n");
#endif
    threshold(thr, -MAX_ERODE, 0);

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    List* queue2 = new List();
    List* queue3 = new List();
    List* queue4 = new List();

    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if((*this)(i, j, k) >= 0) {
                    if(preserve(i, j, k) > 0) {
                        (*this)(i, j, k) = MAX_ERODE;
                    }
                    else {
                        for(int m = 0; m < 6; m++) {
                            if((*this)(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                // (*this)( i, j, k, 1 ) ;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->size() );
    printf("Start erosion to %d...\n", wid);
#endif

    // Perform erosion
    QueueNode* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(), this->getSizeZ(), -1);

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 and queue4 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->size(), curwid);
#endif

        /*
         We first need to assign curwid + 1 to every node in this layer
         */
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            if((*this)(ox, oy, oz) == curwid) {
                ele = queue2->remove();
            }
            else {
                (*this)(ox, oy, oz) = curwid;
                ele = queue2->getNext();
            }
        }
        queue4->reset();
        ele = queue4->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            queue2->prepend(ox, oy, oz);
            ele = queue4->remove();
        }

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex(ox, oy, oz);
            (*scrvol)(ox, oy, oz) = score;

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new List();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

            // printf("%d\n", score);
//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if((*this)(ox, oy, oz) != curwid || (int)(*scrvol)(ox, oy, oz) != score) {
                continue;
            }

            if( (!isSimple(ox, oy, oz)) || isSheetEnd(ox, oy, oz)) {
                // Complex, set to next layer
                (*this)(ox, oy, oz) = curwid + 1;
                queue4->prepend(ox, oy, oz);
                numComplex++;

            }
            else {
                (*this)(ox, oy, oz) = -1;
                numSimple++;

            }
            /* Adding ends */

            // Move its neighboring unvisited node to queue2
            for(int m = 0; m < 6; m++) {
                int nx = ox + neighbor6[m][0];
                int ny = oy + neighbor6[m][1];
                int nz = oz + neighbor6[m][2];
                if((*this)(nx, ny, nz) == 0) {
                    // (*this)( nx, ny, nz, curwid + 1 ) ;
                    queue2->prepend(nx, ny, nz);
                }
            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue
            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if((*this)(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex(nx, ny, nz);

                            if(score != (int)(*scrvol)(nx, ny, nz)) {
                                // printf("Update\n") ;
                                (*scrvol)(nx, ny, nz) = score;
                                // Push to queue
                                gridPoint gp(nx, ny, nz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            break;
        }
    }

    // Finally, clean up
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);

    delete scrvol;

    delete queue2;
    delete queue3;
    delete queue4;

}

/**
 * Normalize to a given range
 */
void Volume::threshold(double thr) {
    threshold(thr, 0, 1, 0, true);
}

void Volume::threshold(double thr, int out, int in) {
    threshold(thr, out, in, out, true);
}

void Volume::threshold(double thr, int out, int in, int boundary,
                       bool markBoundary)
{
    float val;
    for(int i = 0; i < getSizeX(); i++)
        for(int j = 0; j < getSizeY(); j++)
            for(int k = 0; k < getSizeZ(); k++) {
                val = (float)(*this)(i, j, k);
                if(markBoundary) {
                    if(i > 1 && i < getSizeX() - 2 && j > 1
                       && j < getSizeY() - 2 && k > 1 && k < getSizeZ() - 2) {
                        if(val < thr) {
                            (*this)(i, j, k) = out;
                        }
                        else {
                            (*this)(i, j, k) = in;
                        }
                    }
                    else {
                        (*this)(i, j, k) = boundary;
                    }
                }
                else {
                    if(val < thr) {
                        (*this)(i, j, k) = out;
                    }
                    else {
                        (*this)(i, j, k) = in;
                    }
                }
            }
}

void Volume::threshold2(double thr, int out, int in) {
    for(int i = 0; i < getSizeX(); i++)
        for(int j = 0; j < getSizeY(); j++)
            for(int k = 0; k < getSizeZ(); k++) {
                double val = (*this)(i, j, k);
                if(val <= thr) {
                    (*this)(i, j, k) = out;
                }
                else {
                    (*this)(i, j, k) = in;
                }
            }
}

void Volume::normalize(double min, double max) {
    double imin = getMin();
    double imax = getMax();
    double irange = imax - imin;
    double range = max - min;

    int size = getMaxIndex();
    for(int i = 0; i < size; i++) {
        (*this)(i) = ( ((*this)(i) - (float)imin) / (float)irange) * (float)range + (float)min;
    }
}

/* Set data at a pixel */

Volume Volume::getDataRange(int x, int y, int z, int radius) const {
    Volume range(radius * 2 + 1, radius * 2 + 1, radius * 2 + 1);
    for(int xx = x - radius; xx <= x + radius; xx++) {
        for(int yy = y - radius; yy <= y + radius; yy++) {
            for(int zz = z - radius; zz <= z + radius; zz++) {
                range(xx - x + radius, yy - y + radius, zz - z + radius) = (*this)(xx, yy, zz);
            }
        }
    }
    return range;
}

void Volume::toMRCFile(string fname) {
    FILE* fout = fopen(fname.c_str(), "wb");

    // Write header
    int sizeX = getSizeX();
    int sizeY = getSizeY();
    int sizeZ = getSizeZ();
    fwrite(&sizeX, sizeof(int), 1, fout);
    fwrite(&sizeY, sizeof(int), 1, fout);
    fwrite(&sizeZ, sizeof(int), 1, fout);

    int mode = 2;
    fwrite(&mode, sizeof(int), 1, fout);

    int off[3] = {0, 0, 0};

    // Changed: MX == NX instead of MX = NX -1 (similar for Y and Z) because that is how EMAN2 and UCSF Chimera do it
    //int intv[3] = { getSizeX() - 1, getSizeY() - 1, getSizeZ() - 1 } ;
    int intv[3] = {getSizeX(), getSizeY(), getSizeZ()};

    fwrite(off, sizeof(int), 3, fout);
    fwrite(intv, sizeof(int), 3, fout);

    float cella[3] = {getSpacingX() * (float) (getSizeX()), getSpacingY()
            * (float) (getSizeY()),
                      getSpacingZ() * (float) (getSizeZ())};
    float cellb[3] = {90, 90, 90};
    fwrite(cella, sizeof(float), 3, fout);
    fwrite(cellb, sizeof(float), 3, fout);

    int cols[3] = {1, 2, 3};
    fwrite(cols, sizeof(int), 3, fout);

    double dmin = 100000, dmax = -100000;
    int i;
    int size = getMaxIndex();
    for(i = 0; i < size; i++) {
        float val = (float)(*this)(i);
        if(val < dmin) {
            dmin = val;
        }
        if(val > dmax) {
            dmax = val;
        }
    }
    float ds[3] = {(float)dmin, (float)dmax, (float)0};
    fwrite(ds, sizeof(float), 3, fout);

    int zero = 0;
    for(i = 23; i <= 49; i++) {
        fwrite(&zero, sizeof(int), 1, fout);
    }

    float origins[3];
    origins[0] = getOriginX() / getSpacingX() + 0.5f * (float)getSizeX();
    origins[1] = getOriginY() / getSpacingY() + 0.5f * (float)getSizeY();
    origins[2] = getOriginZ() / getSpacingZ() + 0.5f * (float)getSizeZ();

    fwrite(origins, sizeof(float), 3, fout);

    for(i = 53; i <= 256; i++) {
        fwrite(&zero, sizeof(int), 1, fout);
    }

    // Write contents
    for(int z = 0; z < getSizeZ(); z++)
        for(int y = 0; y < getSizeY(); y++)
            for(int x = 0; x < getSizeX(); x++) {
                float d = (float)(*this)(x, y, z);
                fwrite(&d, sizeof(float), 1, fout);
            }

    fclose(fout);
}

// Returns the mean value of all the voxels
float Volume::getMean() {
    int N = getMaxIndex();
    double mass = 0;
    for(int i = 0; i < N; i++)
        mass += (*this)(i);
    float mean = (float)mass / N;
    return mean;
}

// Returns the population standard deviation of the values at all the voxels
float Volume::getStdDev() {
    int N = getMaxIndex();

    //Calculate the standard deviation of all the voxels in the image
    double voxel_sum = 0;
    double voxel_squared_sum = 0;
    float val;

    for(int i = 0; i < N; i++) {
        val = (float)(*this)(i);
        voxel_sum += val;
        voxel_squared_sum += val * val;
    }
    float std_dev = (float)sqrt(
            (voxel_squared_sum - voxel_sum * voxel_sum / N) / N);
    return std_dev;
}

void Volume::Normalize() {
    normalize(0, 1);
}

void Volume::downsample() {
    Volume dest(getSizeX()/2, getSizeY()/2, getSizeZ()/2);

    int radius = 1;

    ProbDistr3D gaussianFilter;
    gaussianFilter.R = radius;
    BinomDistr(gaussianFilter);

    for(int x = radius; x < dest.getSizeX()-radius; x++) {
        for(int y = radius; y < dest.getSizeY()-radius; y++) {
            for(int z = radius; z < dest.getSizeZ()-radius; z++) {
                double val = 0;
                for(int xx = -radius; xx <= radius; xx++) {
                    for(int yy = -radius; yy <= radius; yy++) {
                        for(int zz = -radius; zz <= radius; zz++) {
                            val += (*this)(2*x+xx, 2*y+yy, 2*z+zz) * gaussianFilter.vals[xx+radius][yy+radius][zz+radius] ;
                        }
                    }
                }
                dest(x, y, z) = val;
            }
        }
    }

    *this = dest;
}

void Volume::load(string inputFile) {
//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
          cout<<"DEBUG: Method: Volume::load(string)\033[0m"<<endl;
          cout<<*this<<endl;
//    #endif

    *this = *MRCReaderPicker::pick(inputFile.c_str())->getVolume();

//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
          cout<<"DEBUG: Method: Volume::load(string)\033[0m"<<endl;
          cout<<*this<<endl;
//    #endif

}

void Volume::save(string fileName) {
    int pos = fileName.rfind(".") + 1;
    string extension = fileName.substr(pos, fileName.length() - pos);

    extension = StringUtils::StringToUpper(extension);

    if(extension == "MRC") {
        toMRCFile(fileName.c_str());
    }
    else {
        cout << "Input format " << extension << " not supported!" << endl;
    }
}

Volume Volume::binarySkeletonization(double threshold,
                                                    int minCurveSize,
                                                    int minSurfaceSize)
{
    Skeletonizer skeletonizer(0, 0, 0,
                              DEFAULT_SKELETON_DIRECTION_RADIUS);
//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
          cout<<"DEBUG: Method: Volume::PerformBinarySkeletonizationJu2007(double, int, int)\033[0m"<<endl;
          cout<<*this<<endl;
//    #endif

    Volume outputVol = *skeletonizer.PerformPureJuSkeletonization(*this, "",
            threshold, minCurveSize, minSurfaceSize);

//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
          cout<<"DEBUG: Method: Volume::PerformBinarySkeletonizationJu2007(double, int, int)\033[0m"<<endl;
          cout<<"outputVol->getSize(): "<<outputVol.getSize()<<endl;
          cout<<"outputVol->getNonZeroVoxelCount(): "<<outputVol.getNonZeroVoxelCount()<<endl;
          cout<<"\t"<<threshold
              <<"\t"<<minCurveSize
              <<"\t"<<minSurfaceSize
              <<endl;
          cout<<outputVol<<endl;
//    #endif

    return outputVol;
}

Volume Volume::grayscaleSkeletonization(
        double startDensity, int stepCount, int minCurveSize,
        int minSurfaceSize, int curveRadius, int surfaceRadius,
        int skeletonRadius)
{
    double stepSize = (getMax() - startDensity) / stepCount;
    if(!isZero(stepSize)) {
        Skeletonizer skeletonizer(0,
                curveRadius, surfaceRadius, skeletonRadius);
        Volume outputVol =
                *skeletonizer.PerformImmersionSkeletonizationAndPruning(*this,
                        NULL, startDensity, getMax(), stepSize, 0, 0,
                        minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0,
                        DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);
        return outputVol;
    }
}

void Volume::buildHistogram(int binCount) {
    histogram.clear();
    for(int i = 0; i < binCount; i++) {
        histogram.push_back(0);
    }

    float minVal = getMin();
    float maxVal = getMax();
    float binSize = (maxVal - minVal)/(float)(binCount - 1);
    int binIx;
    for(unsigned int i = 0; i < getSizeX(); i++) {
        for(unsigned int j = 0; j < getSizeY(); j++) {
            for(unsigned int k = 0; k < getSizeZ(); k++) {
                binIx = (int)(((*this)(i,j,k) - minVal)/binSize);
                histogram[binIx]++;
            }
        }
    }
}

int Volume::getHistogramBinValue(int binIx) {
    return histogram[binIx];

}
}
