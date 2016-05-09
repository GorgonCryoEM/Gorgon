/*
 * RendererBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "RendererBase.h"
#include "Foundation/StringUtils.h"

namespace Visualization {

    RendererBase::RendererBase()
            : IsoSurface((Volume)(*this))
    {}

    void RendererBase::load(string fileName) {
//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   RendererBase.cpp"<<endl;
              cout<<"DEBUG: Method: RendererBase::load(string)\033[0m"<<endl;
              cout<<(Volume)(*this)<<endl;
//        #endif

        Volume::load(fileName);

//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   RendererBase.cpp"<<endl;
              cout<<"DEBUG: After load()\033[0m"<<endl;
              cout<<(Volume)(*this)<<endl;
//        #endif

        #ifdef _WIN32
            glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
        #endif

    }

    void RendererBase::save(string fileName) {
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);

        extension = StringUtils::StringToUpper(extension);

        if(strcmp(extension.c_str(), "MRC") == 0) {
            toMRCFile((char *)fileName.c_str());
        } else {
            printf("Input format %s not supported!\n", extension.c_str());
        }
    }

    void RendererBase::draw(int subSceneIndex, bool selectEnabled) {
    }

    float RendererBase::getMinPos(int i) const {
        return 0.0;
    }

    float RendererBase::getMaxPos(int i) const {
        float result;
        switch(i) {
            case 0:
                result = float(getSizeX()-1);
                break;
            case 1:
                result = float(getSizeY()-1);
                break;
            case 2:
                result = float(getSizeZ()-1);
                break;
        }

        return result;
    }
}
