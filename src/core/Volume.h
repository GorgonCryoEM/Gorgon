#ifndef CORE_SKELETON_MAKER_VOLUME_DATA_H
#define CORE_SKELETON_MAKER_VOLUME_DATA_H

#include <vector>
#include "Dim3D.h"

using namespace std;
//using namespace Core;

namespace Core {
    class VolumeData {
    public:
        VolumeData();
        VolumeData(int sizeX, int sizeY, int sizeZ, float val=0.0);

        bool cmp(const VolumeData& obj)  const;

        int getSizeX() const;
        int getSizeY() const;
        int getSizeZ() const;
        int getSize() const;
        Dim3D<int> getSizeObj() const;
        float getSpacingX() const;
        float getSpacingY() const;
        float getSpacingZ() const;
        float getOriginX() const;
        float getOriginY() const;
        float getOriginZ() const;

        float getDataAt(int x, int y, int z) const;
        float getDataAt(int index) const;
        int getIndex(int x, int y, int z) const;
        int getMaxIndex() const;

        void setSpacing(float spacingX, float spacingY, float spacingZ);
        void setOrigin(float originX, float originY, float originZ);
        void setSpacing(Dim3D<float>);
        void setOrigin(Dim3D<float>);
        void setDataAt(int x, int y, int z, float value);
        void setDataAt(int index, float value);
    private:
        void setSize(int sizeX, int sizeY, int sizeZ);
    protected:
        Dim3D<int> size;
        Dim3D<float> spacing;
        Dim3D<float> origin;
        vector<float> data;

        friend ostream& operator<<(ostream& out, const VolumeData& obj){
          return out<<"\033[33m"
              <<"size:    "<<obj.size
              <<"spacing: "<<obj.spacing
              <<"origin:  "<<obj.origin
              <<"size:    "<<obj.data.size()
              <<"\033[0m"<<endl;
        }
    };
}


#endif
