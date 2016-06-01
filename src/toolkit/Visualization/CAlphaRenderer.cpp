/*
 * CAlphaRenderer.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "CAlphaRenderer.h"
#include "Shapes.h"
#include "ProteinMorph/HermiteCurve.h"
#include "Foundation/OpenGLUtils.h"

namespace Visualization {

    CAlphaRenderer::CAlphaRenderer() {
        NUM_SEGMENTS = 10;
        NUM_SLICES = 10;
        HELIX_HERMITE_FACTOR = 4.7;
        HELIX_ALPHA = 32.0 * PI/180.0;
        HELIX_BETA = -11.0 * PI/180.0; // these three values taken from molscript code
        HELIX_WIDTH = 4.0;

        STRAND_HERMITE_FACTOR = .5;
        LOOP_RADIUS = .25;

        renderingType = 1;
        thinRibbThickness = .05;

        hlt_r = 1.0;
        hlt_g = 1.0;
        hlt_b = 1.0;
        hlt_a = 1.0;
    }

    CAlphaRenderer::~CAlphaRenderer() {
    }

    vector<int> CAlphaRenderer::getSelectedHelixIndices(){
        return selectedHelixIndices;
    }

    PDBAtom * CAlphaRenderer::addAtom(PDBAtom atom) {
        atoms[atom.GetHashKey()] = atom;
        return &atoms[atom.GetHashKey()];
    }

    void CAlphaRenderer::addBond(PDBBond bond) {
        bonds.push_back(bond);
    }

    void CAlphaRenderer::addSideChainBond(PDBBond bond) {
        sidechainBonds.push_back(bond);
    }

    void CAlphaRenderer::drawBackboneModel(int subSceneIndex, bool selectEnabled) {
        GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

        selectEnabled = true;

//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   CAlphaRenderer.cpp"<<endl;
              cout<<"DEBUG: Method: CAlphaRenderer::drawBackboneModel(int, bool)\033[0m"<<endl;
              cout<<"...subSceneIndex, selectEnabled...: "<<subSceneIndex<<", "<<selectEnabled<<endl;
              cout<<"......atoms.size(): "<<atoms.size()<<endl;
              cout<<"......bonds.size(): "<<bonds.size()<<endl;
//        #endif


        if(subSceneIndex == 0) { // Drawing Atoms
            if(selectEnabled) {
                atomHashKeys.clear();
                glPushName(0);
                glPushName(0);
            }
            for (AtomMapType::iterator it = atoms.begin(); it != atoms.end(); it++) {
                if(it->second.GetName() == "CA") {
                    glPushAttrib(GL_LIGHTING_BIT);
                    if(it->second.GetSelected()) {
                        glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                        glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                    } else {
                        OpenGLUtils::SetColor(it->second.GetColorR(), it->second.GetColorG(), it->second.GetColorB(), it->second.GetColorA());
                    }

                    if(selectEnabled){
                        //TODO: possibly implement mouse picking using ray intersection
                        atomHashKeys.push_back(it->first); // adding the atom hash key as an element
                        glLoadName(static_cast<GLuint>( atomHashKeys.size() - 1)); // the index of the element just added
                    }
                    if(1) {
                        cout<<".........drawSphere: "<<it->second.GetPosition()<<"\t"<<it->second.GetAtomRadius()<<endl;
                        drawSphere(it->second.GetPosition(), it->second.GetAtomRadius() * 0.3);
                    }

                    glPopAttrib();
                }

            }
            if(selectEnabled) {
                glPopName();
                glPopName();
            }
        } else if(subSceneIndex == 1) { // Drawing Bonds
            if(selectEnabled) {
                glPushName(1);
                glPushName(0);
            }
            for(int i=0; i < (int)bonds.size(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                if(bonds[i].GetSelected()) {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                }

                if(selectEnabled){
                    glLoadName(i);
                }
                float length = (atoms[bonds[i].GetAtom0Ix()].GetPosition() - atoms[bonds[i].GetAtom1Ix()].GetPosition()).length();
                if(length > 4.2) {
                    OpenGLUtils::SetColor(1.0, 0, 0, 1.0);
                }

                if(length < 3.3) {
                    OpenGLUtils::SetColor(0, 0, 1.0, 1.0);
                }

                if(1) {
                    cout<<".........drawCylinder: "<<atoms[bonds[i].GetAtom0Ix()].GetPosition()<<"\t"<<atoms[bonds[i].GetAtom1Ix()].GetPosition()<<endl;
                    drawCylinder(atoms[bonds[i].GetAtom0Ix()].GetPosition(), atoms[bonds[i].GetAtom1Ix()].GetPosition(), 0.1, 10, 2);
                }
                glPopAttrib();
            }
            if(selectEnabled) {
                glPopName();
                glPopName();
            }
        } else if(subSceneIndex == 2) { // Drawing spheres to cover up the cylinder edges
            for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
                if(1) {
                    cout<<".........drawSphere: "<<i->second.GetPosition()<<endl;
                    drawSphere(i->second.GetPosition(), 0.1);
                }
            }
        }
    }

    void CAlphaRenderer::drawRibbonModel(int subSceneIndex, bool selectEnabled) {
        if(selectEnabled) {
            glPushName(subSceneIndex);
            glPushName(0);
        }
        //GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};
        //GLfloat frontColor[4] = {1.0, 0.0, 0.0, 1.0};
        //GLfloat backColor[4] = {0.0, 0.0, 1.0, 1.0};
        GLfloat emissionColor[4] = {hlt_r, hlt_g, hlt_b, hlt_a};
        GLfloat frontColor[4] = {1.0, 0.0, 0.0, 1.0};
        GLfloat backColor[4] = {0.0, 0.0, 1.0, 1.0};
        vector<int> PDBIndices;

        switch(subSceneIndex) {
        case 0: // Helices

            //if(subSceneIndex == 0){
            for(unsigned int i = 0; i < corrs.size(); ++i){
                int SSEIndex = corrs[i].second;
                for(unsigned int k = 0; k < selectedSSEHelices.size(); ++k){
                    if(selectedSSEHelices[k] == SSEIndex){
                        PDBIndices.push_back(corrs[i].first);
                    }
                }
            }

            for(int i = 0; i < aHelices.size(); i++) {
                if(selectEnabled){
                    glLoadName(i);
                }

                glPushAttrib(GL_LIGHTING_BIT);

                Secel currentSecel = aHelices[i];

                if(currentSecel.selected == true){
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                }
                map<int, Vec3F >::iterator iter = helixColors.begin();
                iter = helixColors.find(i);
                if(iter != helixColors.end()){

                    OpenGLUtils::SetColor(helixColors[i][0], helixColors[i][1], helixColors[i][2], 1.0);
                }else{
                    OpenGLUtils::SetColor(0.8,0.8,0.8,1.0);
                }

                if(currentSecel.atomHashes.size() > 0){

                    PDBAtom firstAtom = atoms.find(currentSecel.atomHashes[0])->second;
                    PDBAtom lastAtom = atoms.find(currentSecel.atomHashes[currentSecel.atomHashes.size()-1])->second;
                    Vec3F preSecelAtomPos = atoms.find(firstAtom.GetPrevCAHash())->second.GetPosition();
                    Vec3F postSecelAtomPos = atoms.find(lastAtom.GetNextCAHash())->second.GetPosition();

                    vector<Vec3F> points = createPointVector(firstAtom, lastAtom);
                    vector<Vec3F> tangents = vector<Vec3F>(points);
                    vector<Vec3F> axes = vector<Vec3F>(points);
                    vector<Vec3F> interpPoints = vector<Vec3F>((points.size()-1)*NUM_SEGMENTS + 1);
                    int flatSlices = 2;
                    int rptsize = interpPoints.size()*4;
                    switch (renderingType){
                        case 0:
                            rptsize = interpPoints.size()*flatSlices;
                            break;
                        case 1:
                            rptsize = interpPoints.size()*4;
                            break;
                        default:
                            rptsize = interpPoints.size()*4;
                            break;
                    }
                    vector<Vec3F> renderingPoints(rptsize);
                    vector<Vec3F> renderingNormals(renderingPoints.size());
                    /*vector<Vector3DFloat> renderingPoints(interpPoints.size()*NUM_SLICES);
                    vector<Vector3DFloat> renderingNormals(renderingPoints.size());*/

                    HermiteCurve curve;
                    Vec3F m0, m1;

                    createHelixAxesTangentsAndPoints(axes, tangents, interpPoints, points, preSecelAtomPos, postSecelAtomPos, HELIX_ALPHA, HELIX_BETA, HELIX_HERMITE_FACTOR);

                    for(unsigned int x = 0; x < points.size()-1; ++x){

                        m0 = tangents[x];
                        m1 = tangents[x+1];

                        curve.setCurve(points[x], points[x+1], m0, m1);

                        // used in rendering a helix as glowing if selected
                        vector<Vec3F> selectedBoxPositions(8);

                        float halfwidth = HELIX_WIDTH/2.0;
                        float halfthickness = LOOP_RADIUS;
                        Vec3F lastPos = points[x];
                        //int NUM_SECTIONS = 10;
                        for (int sect = 0; sect <= NUM_SEGMENTS; ++sect){
                            if(sect == 0 && x != 0){
                                continue;
                            }
                            double tsect = ((double)sect)/((double)NUM_SEGMENTS);
                            Vec3F nextPos = curve.getPos(tsect);

                            Vec3F currentAxis = axes[x]*(1.0-tsect) + axes[x+1]*tsect;
                            currentAxis.normalize();

                            Vec3F curnormal = curve.getTangent(tsect);
                            curnormal = curnormal^currentAxis;
                            curnormal.normalize();

                            if(x == 0){
                                halfwidth = LOOP_RADIUS + (0.5 * HELIX_WIDTH - LOOP_RADIUS) * 0.5 * (-1*cos(PI*tsect) + 1.0);
                            } else if (x == points.size() - 2){
                                halfwidth = LOOP_RADIUS + (0.5 * HELIX_WIDTH - LOOP_RADIUS) * 0.5 * (cos(PI*tsect) + 1.0);
                            }

                            //for(int y = 0; y < NUM_SLICES; ++y){
                            //  renderingPoints[(x*NUM_SEGMENTS + sect) + y*interpPoints.size()] = nextPos + currentAxis*halfwidth*cos(y*2*PI/NUM_SLICES) + curnormal*halfthickness*sin(y*2*PI/NUM_SLICES);
                            //  renderingNormals[(x*NUM_SEGMENTS + sect) + y*interpPoints.size()] = currentAxis*halfwidth*cos(y*2*PI/NUM_SLICES)*sin(y*2*PI/NUM_SLICES)/(cos(y*2*PI/NUM_SLICES)+sin(y*2*PI/NUM_SLICES))
                            //      + (curnormal*halfthickness*sin(y*2*PI/NUM_SLICES)*cos(y*2*PI/NUM_SLICES))/(cos(y*2*PI/NUM_SLICES)+sin(y*2*PI/NUM_SLICES));
                            //  renderingNormals[(x*NUM_SEGMENTS + sect) + y*interpPoints.size()].normalize();
                            //}


                            switch (renderingType) {
                                case 0:
                                    for (int q = 0; q < flatSlices; ++q){
                                        renderingPoints[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = nextPos + currentAxis*halfwidth*(q%3 == 0 ? -1 : 1); //+ curnormal*hlt_r*pow(-1.0, q/2);
                                        renderingNormals[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = curnormal;//*(q%3 == 0 ? -1 : 1);
                                        renderingNormals[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()].normalize();
                                    }
                                    break;
                                case 1:
                                    for(int q = 0; q < 4; ++q){
                                        renderingPoints[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = nextPos + currentAxis*halfwidth*pow(-1.0, q/2)
                                            + curnormal*halfthickness*(q%3 == 0 ? -1 : 1);
                                        renderingNormals[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = curnormal*(q%3 == 0 ? -1 : 1);
                                    }
                                    break;
                                default:
                                    cout << "should not have reached default case in ahelices rendering method" << endl;


                            }

                            //for(int q = 0; q < 4; ++q){
                            //  switch (renderingType){
                            //      case 0:
                            //          renderingPoints[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = nextPos + currentAxis*halfwidth*pow(-1.0, q/2); //+ curnormal*thinRibbThickness*(q%3 == 0 ? -1 : 1);
                            //          renderingNormals[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = curnormal*(q%3 == 0 ? -1 : 1);
                            //          break;
                            //      case 1:
                            //          renderingPoints[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = nextPos + currentAxis*halfwidth*pow(-1.0, q/2) + curnormal*halfthickness*(q%3 == 0 ? -1 : 1);
                            //          renderingNormals[(x*NUM_SEGMENTS + sect) + q*interpPoints.size()] = curnormal*(q%3 == 0 ? -1 : 1);
                            //          break;
                            //      default:
                            //          break;
                            //  }

                            //}

                            //double HLT_FACTOR = 1.05;
                        }
                    }
                    switch (renderingType){
                        case 0:
                            drawTube(renderingPoints, renderingNormals, interpPoints.size() - 1, flatSlices);
                            break;
                        case 1:
                            drawTube(renderingPoints, renderingNormals, interpPoints.size() - 1, 4);
                            break;
                        default:
                            break;
                    }
                    //DrawTube(renderingPoints, renderingNormals, interpPoints.size() - 1, 4);
                    //DrawTube(renderingPoints, renderingNormals, interpPoints.size() - 1, NUM_SLICES);
                }
                glPopAttrib();

                if(aHelices[i].selected == true){
                    glPushAttrib(GL_LIGHTING_BIT);

                    if(featureVecs.size() > 0){
                        OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                        drawSphere(featureVecs[i].first, 1.0);
                        OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                        drawSphere(featureVecs[i].second, 1.0);
                    }else{
                        OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                        drawSphere(atoms[aHelices[i].atomHashes[0]].GetPosition(), 1.0);
                        OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                        drawSphere(atoms[aHelices[i].atomHashes[aHelices[i].atomHashes.size()-1]].GetPosition(), 1.0);
                    }

                    glPopAttrib();
                    Vec3F pos1 = atoms[aHelices[i].atomHashes[0]].GetPosition();
                    Vec3F pos2 = atoms[aHelices[i].atomHashes[aHelices[i].atomHashes.size()-1]].GetPosition();
                    printf("Drawing PDB Spheres at PDB ID %d with end #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", i+1, pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z());

                    fflush(stdout);
                }

                for(unsigned int j = 0; j < PDBIndices.size(); ++j){
                    if(PDBIndices[j] == i){
                        glPushAttrib(GL_LIGHTING_BIT);
                        OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                        drawSphere(atoms[aHelices[i].atomHashes[0]].GetPosition(), 1.0);
                        OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                        drawSphere(atoms[aHelices[i].atomHashes[aHelices[i].atomHashes.size()-1]].GetPosition(), 1.0);
                        glPopAttrib();
                    }
                }
            }
            //}
            break;
        case 1: // Strands
            for (unsigned int i = 0; i < bStrands.size(); ++i){
                if(selectEnabled){
                    glLoadName(i);
                }

                int atom_counter = 0;
                PDBAtom lastEnd;

                HermiteCurve curve;
                Vec3F m0, m1, dir1, dir2;

                glPushAttrib(GL_LIGHTING_BIT);

                Secel currentSecel = bStrands[i];

                if(currentSecel.selected == true){
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                }

                if(currentSecel.atomHashes.size() > 0){
                    PDBAtom firstAtom = atoms.find(currentSecel.atomHashes[0])->second;
                    PDBAtom lastAtom = atoms.find(currentSecel.atomHashes[currentSecel.atomHashes.size()-1])->second;
                    Vec3F preSecelAtomPos = atoms.find(firstAtom.GetPrevCAHash())->second.GetPosition();
                    Vec3F postSecelAtomPos = atoms.find(lastAtom.GetNextCAHash())->second.GetPosition();

                    vector<Vec3F> points = createPointVector(firstAtom, lastAtom);
                    int num_interp_points = (points.size() - 1)*NUM_SEGMENTS + 1;
                    int num_rendering_points = num_interp_points*4;
                    switch (renderingType){
                        case 0:
                            num_rendering_points = num_interp_points * 2;
                            break;
                        case 1:
                            num_rendering_points = num_interp_points * 4;
                            break;
                        default:
                            cout << "bstrands" << endl;
                    }
                    vector<Vec3F> renderingPoints(num_rendering_points);
                    vector<Vec3F> normals = createStrandNormals(points, preSecelAtomPos, postSecelAtomPos);
                    vector<Vec3F> renderingNormals(renderingPoints);
                    double arrowhead_factor = 1.0;
                    //vector<Vector3DFloat> boxpositions(8);
                    //vector<Vector3DFloat> boxnormals(8);

                    bool LAPLACIAN_SMOOTHING = true;
                    int SMOOTHING_STEPS = 1;
                    if(LAPLACIAN_SMOOTHING){
                        points = laplacianSmoothing(points, SMOOTHING_STEPS);
                    }

                    for(unsigned int i = 0; i < points.size()-1; ++i){
                        if(i == 0){
                            m0 = points[i+1] - points[i];
                        } else {
                            if(i + 2 < points.size()){
                                m0 = points[i+2] - points[i];
                            } else {
                                m0 = postSecelAtomPos - points[i];
                            }
                        }

                        if(i + 3 >= points.size()){
                            m1 = postSecelAtomPos - points[i];
                        } else {
                            m1 = points[i+3] - points[i+1];
                        }

                        m0 = m0*STRAND_HERMITE_FACTOR;
                        m1 = m1*STRAND_HERMITE_FACTOR;

                        if(i == 0){
                            dir2 = points[1] - points[0];
                            dir2.normalize();
                        }

                        dir1 = dir2;
                        if(i + 2 < points.size()){
                            dir2 = points[i+2] - points[i];
                        } else {
                            dir2 = postSecelAtomPos - points[i];
                        }

                        dir2.normalize();

                        curve.setCurve(points[i], points[i+1], m0, m1);

                        float WIDTH = 1.3;
                        float THICKNESS = LOOP_RADIUS;
                        Vec3F lastPos = points[i];
                        Vec3F direction = dir1;
                        Vec3F currentNormal = normals[i];
                        Vec3F side = currentNormal^direction;
                        side.normalize();

                        for (int sect = 0; sect <= NUM_SEGMENTS; ++sect){
                            if (sect == 0 && i != 0){
                                continue;
                            }

                            double tsect = ((double)sect)/((double)NUM_SEGMENTS);
                            if(i > points.size() - 3){
                                arrowhead_factor = 2.0*(1-tsect);
                            }
                            direction = dir1*(1.0 - tsect) + dir2*tsect;
                            currentNormal = normals[i]*(1.0 - tsect) + normals[i+1]*(tsect);
                            side = currentNormal^direction;
                            side.normalize();
                            Vec3F nextPos = curve.getPos(tsect);

                            switch(renderingType){
                                    case 0:
                                        for(int q = 0; q < 2; ++q){
                                            renderingPoints[i*NUM_SEGMENTS + sect + q*num_interp_points] = nextPos + side*(0.5*WIDTH*arrowhead_factor + LOOP_RADIUS/2.0)*(q%3 == 0 ? -1 : 1);
                                            renderingNormals[i*NUM_SEGMENTS + sect + q*num_interp_points] = currentNormal;//*pow(-1.0, q/2);
                                        }
                                        break;
                                    case 1:
                                        for(int q = 0; q < 4; ++q){
                                            renderingPoints[i*NUM_SEGMENTS + sect + q*num_interp_points] = nextPos + side*(0.5*WIDTH*arrowhead_factor + LOOP_RADIUS/2.0)*(q%3 == 0 ? -1 : 1)
                                                + currentNormal*0.5*THICKNESS*pow(-1.0, q/2);
                                            renderingNormals[i*NUM_SEGMENTS + sect + q*num_interp_points] = currentNormal*pow(-1.0, q/2);
                                        }
                                        break;
                                    default:
                                        std::cout << "You've reached the default.  You should never reach the default." << std::endl;
                                        break;
                            }


                        }

                        if (i == points.size() - 3){
                            arrowhead_factor = 2.0;
                        }
                    }
                    switch (renderingType){
                        case 0:
                            drawTube(renderingPoints, renderingNormals, num_interp_points - 1, 2);
                            break;
                        case 1:
                            drawTube(renderingPoints, renderingNormals, num_interp_points - 1, 4);
                            break;
                        default:
                            cout << "should not have reached bstrand drawtube default" << endl;
                    }

                }
                glPopAttrib();

                if(currentSecel.selected == true){
                    glPushAttrib(GL_LIGHTING_BIT);

                    Vec3F pos1 = atoms[currentSecel.atomHashes[0]].GetPosition();
                    Vec3F pos2 = atoms[currentSecel.atomHashes[currentSecel.atomHashes.size()-1]].GetPosition();
                    printf("Drawing PDB Spheres at PDB ID %d with end #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", i+1, pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z());

                    /*if(featureVecs.size() > 0){
                    OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                    drawSphere(featureVecs[i].get<0>(), 1.0);
                    OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                    drawSphere(featureVecs[i].get<1>(), 1.0);
                    }else{*/
                    OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                    drawSphere(pos1, 1.0);
                    OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                    drawSphere(pos2, 1.0);
                    /*}*/

                    glPopAttrib();

                    fflush(stdout);
                }
            }
            break;
            //}
            //else if (subSceneIndex == 2){
        case 2: // Loops
            for (unsigned int i = 0; i < loops.size(); ++i){
                if(selectEnabled){
                    glLoadName(i);
                }
                //GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

                int atom_counter = 0;
                PDBAtom lastEnd;

                HermiteCurve curve;
                Vec3F m0, m1;

                double HERMITE_FACTOR = 0.5;

                glPushAttrib(GL_LIGHTING_BIT);
                Secel currentSecel = loops[i];

                if(currentSecel.selected == true){
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                }

                if(currentSecel.atomHashes.size() > 1){
                    PDBAtom firstAtom = atoms.find(currentSecel.atomHashes[0])->second;
                    PDBAtom lastAtom = atoms.find(currentSecel.atomHashes[currentSecel.atomHashes.size()-1])->second;
                    Vec3F preSecelAtomPos = atoms.find(firstAtom.GetPrevCAHash())->second.GetPosition();
                    Vec3F postSecelAtomPos = atoms.find(lastAtom.GetNextCAHash())->second.GetPosition();

                    vector<Vec3F> points = createPointVector(firstAtom, lastAtom);
                    vector<Vec3F> normals(points.size());
                    normals = createStrandNormals(points, preSecelAtomPos, postSecelAtomPos);

                    // generate smoothed interpolated points
                    vector<Vec3F> interpolatedPoints = interpolateLoopPoints(points, preSecelAtomPos, postSecelAtomPos, NUM_SEGMENTS);
                    int ptsize = interpolatedPoints.size();

                    // create vectors to hold the vertices and normals of our loop polygon
                    int renderingsize = (ptsize)*NUM_SLICES;
                    switch (renderingType){
                        case 0:
                            renderingsize = ptsize*2;
                            break;
                        case 1:
                            renderingsize = (ptsize)*NUM_SLICES;
                            break;
                        default:
                            break;
                    }
                    vector<Vec3F> renderingPoints(renderingsize);
                    vector<Vec3F> renderingNormals(renderingPoints.size());

                    Vec3F nextVector = interpolatedPoints[1]-interpolatedPoints[0];
                    Vec3F previousVector = preSecelAtomPos - interpolatedPoints[0];
                    if(previousVector.length() < .001){
                        if(2 < interpolatedPoints.size()){
                            previousVector = interpolatedPoints[2] - interpolatedPoints[0];
                        } else {
                            previousVector = postSecelAtomPos - interpolatedPoints[0];
                        }
                    }
                    Vec3F curAxis = nextVector^previousVector;
                    Vec3F curNormal = nextVector^curAxis;
                    curNormal.normalize();
                    Vec3F curPos = interpolatedPoints[0];
                    nextVector.normalize();

                    // generate first stack of points
                    Vec3F outward = normals[0] ^ (interpolatedPoints[1]-interpolatedPoints[0]);
                    outward.normalize();
                    switch (renderingType){
                        case 0:
                            for(unsigned int k = 0; k < 2; ++k){
                                renderingPoints[k*ptsize] = curPos + outward*LOOP_RADIUS*(k%2 == 0 ? 1 : -1);
                                renderingNormals[k*ptsize] = normals[0];
                            }
                            break;
                        case 1:
                            for(unsigned int k = 0; k < NUM_SLICES; ++k){
                                Vec3F outwardNormal = (curNormal.rotate(nextVector, ((double)k)*2*PI/((double)NUM_SLICES)));
                                outwardNormal.normalize();
                                renderingPoints[k*(ptsize)] = curPos+outwardNormal*LOOP_RADIUS;
                                renderingNormals[k*(ptsize)] = outwardNormal; //renderingPoints[j*(ptsize)]-curPos;
                            }
                            break;
                        default:
                            break;
                    }

                    for(unsigned int j = 1; j < ptsize; ++j){
                        Vec3F nextVector, previousVector;
                        curPos = interpolatedPoints[j];
                        if(j == ptsize - 1){
                            nextVector = postSecelAtomPos - interpolatedPoints[j];
                        } else {
                            nextVector = interpolatedPoints[j+1]-interpolatedPoints[j];
                        }

                        if(j == 0){
                            previousVector = interpolatedPoints[0] - preSecelAtomPos;
                        } else {
                            previousVector = interpolatedPoints[j-1] - interpolatedPoints[j];
                        }
                        Vec3F newAxis = nextVector^previousVector;
                        if (newAxis.length() < .0001){
                            int pix = j - 2;
                            int nix = j + 2;
                            if (pix < 0){
                                pix = 0;
                            }
                            if (nix >= ptsize){
                                nix = ptsize - 1;
                            }

                            newAxis = (interpolatedPoints[nix] - interpolatedPoints[j])^(interpolatedPoints[pix] - interpolatedPoints[j]);
                        }
                        curAxis = newAxis;
                        double alpha = asin(curAxis.length()/(nextVector.length()*previousVector.length()));
                        curAxis.normalize();
                        curNormal.normalize();
                        if (nextVector.length() > .001 && previousVector.length() > .001){
                            curNormal = curNormal.rotate(curAxis, -1*alpha);
                        }

                        nextVector.normalize();

                        Vec3F dirtemp;
                        if (j + 1 < ptsize){
                            dirtemp = (interpolatedPoints[j+1] - interpolatedPoints[j]);
                        } else {
                            dirtemp = interpolatedPoints[j] - interpolatedPoints[j-1];
                        }
                        float tsect = ((float)(j%NUM_SEGMENTS))/((float)NUM_SEGMENTS);
                        Vec3F outward = (normals[j/NUM_SEGMENTS]*(1.0 - tsect) + normals[j/NUM_SEGMENTS + 1]*(tsect)) ^ dirtemp;
                        outward.normalize();
                        switch(renderingType){
                            case 0:
                                for(unsigned int k = 0; k < 2; ++k){
                                    renderingPoints[j+k*(ptsize)] = curPos+outward*LOOP_RADIUS*(k%2 == 0 ? 1 : -1);
                                    renderingNormals[j+k*(ptsize)] = normals[j/NUM_SEGMENTS];
                                }
                                break;
                            case 1:
                                for(unsigned int k = 0; k < NUM_SLICES; ++k){
                                    Vec3F outwardNormal = (curNormal.rotate(nextVector, ((double)k*2*PI)/NUM_SLICES));
                                    outwardNormal.normalize();
                                    renderingPoints[j+k*(ptsize)] = curPos+outwardNormal*LOOP_RADIUS;
                                    renderingNormals[j+k*(ptsize)] = outwardNormal;
                                }
                                break;
                            default:
                                cout << "shouldn't've gotten to the loop rendering default case" << endl;
                                break;
                        }


                    }
                    float r,g,b,a;
                    if(currentSecel.selected == true){
                        OpenGLUtils::GetColor(r,g,b,a);
                        OpenGLUtils::SetColor(hlt_r, hlt_g, hlt_b, hlt_a);
                    }
                    switch (renderingType){
                        case 0:
                            drawTube(renderingPoints, renderingNormals, interpolatedPoints.size() - 1, 2);
                            break;
                        case 1:
                            drawTube(renderingPoints, renderingNormals, interpolatedPoints.size() - 1, NUM_SLICES);
                            break;
                        default:
                            cout << "shouldn't have reached loop drawtube default case" << endl;
                    }
                    if(currentSecel.selected == true){
                        OpenGLUtils::SetColor(r,g,b,a);
                    }
                }

                glPopAttrib();

                if(currentSecel.selected == true){
                    glPushAttrib(GL_LIGHTING_BIT);

                    Vec3F pos1 = atoms[currentSecel.atomHashes[0]].GetPosition();
                    Vec3F pos2 = atoms[currentSecel.atomHashes[currentSecel.atomHashes.size()-1]].GetPosition();
                    printf("Drawing PDB Spheres at PDB ID %d with end #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", i+1, pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z());

                    /*if(featureVecs.size() > 0){
                    OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                    drawSphere(featureVecs[i].get<0>(), 1.0);
                    OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                    drawSphere(featureVecs[i].get<1>(), 1.0);
                    }else{*/
                    OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                    drawSphere(pos1, 1.0);
                    OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                    drawSphere(pos2, 1.0);
                    /*}*/

                    glPopAttrib();

                    fflush(stdout);
                }
            }
            break;
        }

        if(selectEnabled) {
            glPopName();
            glPopName();
        }
    }

    void CAlphaRenderer::drawSideChainModel(int subSceneIndex, bool selectEnabled) {
        GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};
        float r,g,b,a;

        if(subSceneIndex == 0) { // Drawing Atoms
            if(selectEnabled) {
                atomHashKeys.clear();
                glPushName(0);
                glPushName(0);
            }
            for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                if(i->second.GetSelected()) {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                } else {
                    i->second.GetColor(r, g, b, a);
                    OpenGLUtils::SetColor(r,g,b,a);
                }

                if(selectEnabled){
                    //TODO: possibly implement mouse picking using ray intersection
                    atomHashKeys.push_back(i->first); // adding the hash key
                    glLoadName(static_cast<GLuint>(atomHashKeys.size() - 1)); // using the index of the element just added
                }
                if(i->second.GetVisible()) {
                    drawSphere(i->second.GetPosition(), i->second.GetAtomRadius() * 0.3);
                }

                glPopAttrib();

            }
            if(selectEnabled) {
                glPopName();
                glPopName();
            }
        } else if(subSceneIndex == 1) { // Drawing Bonds
            if(selectEnabled) {
                glPushName(1);
                glPushName(0);
            }
            Vec3F v1, vc, v2;


            for(int i=0; i < (int)sidechainBonds.size(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                if(selectEnabled){
                    glLoadName(i);
                }

                if(atoms[sidechainBonds[i].GetAtom0Ix()].GetVisible() && atoms[sidechainBonds[i].GetAtom1Ix()].GetVisible()) {
                    v1 = atoms[sidechainBonds[i].GetAtom0Ix()].GetPosition();
                    v2 = atoms[sidechainBonds[i].GetAtom1Ix()].GetPosition();
                    if(sidechainBonds[i].GetSelected()) {
                        glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                        glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
                        drawCylinder(v1, v2, 0.1, 6, 2);
                    } else {
                        vc = (v1 + v2) * 0.5;
                        atoms[sidechainBonds[i].GetAtom0Ix()].GetColor(r, g, b, a);
                        OpenGLUtils::SetColor(r,g,b,a);
                        drawCylinder(v1, vc, 0.1, 6, 2);
                        atoms[sidechainBonds[i].GetAtom1Ix()].GetColor(r, g, b, a);
                        OpenGLUtils::SetColor(r,g,b,a);
                        drawCylinder(vc, v2, 0.1, 6, 2);
                    }
                }
                glPopAttrib();
            }
            if(selectEnabled) {
                glPopName();
                glPopName();
            }
        } else if(subSceneIndex == 2) { // Drawing spheres to cover up the cylinder edges
            for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                i->second.GetColor(r, g, b, a);
                OpenGLUtils::SetColor(r,g,b,a);
                drawSphere(i->second.GetPosition(), 0.1);
                glPopAttrib();
            }
        }
    }

    void CAlphaRenderer::draw(int subSceneIndex, bool selectEnabled) {
//        switch(displayStyle) {
//            case CALPHA_DISPLAY_STYLE_BACKBONE: // Backbone only
                drawBackboneModel(subSceneIndex, selectEnabled);
//                break;
//            case CALPHA_DISPLAY_STYLE_RIBBON: // Ribbon mode
                drawRibbonModel(subSceneIndex, selectEnabled);
//                break;
//            case CALPHA_DISPLAY_STYLE_SIDE_CHAIN: // Side chains
                drawSideChainModel(subSceneIndex, selectEnabled);
//                break;
//        }
    }

    PDBAtom * CAlphaRenderer::getAtomFromHitStack(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
        if(subsceneIndex == 0) {
            //TODO: possibly implement mouse picking using ray intersection
            AtomMapType::iterator it = atoms.find(atomHashKeys.at(ix0));
            if (it == atoms.end())
                return NULL;
            else
                return &(it->second);
        }
        return NULL;
    }

    void CAlphaRenderer::loadFile(string fileName) {
//        RendererBase::load(fileName);
        atoms.clear();
        bonds.clear();
        atoms = PDBReader::ReadAtomPositions(fileName);

        // Keeping only C-Alpha atoms
        vector<unsigned long long> eraseKeys;
        eraseKeys.clear();

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            if(i->second.GetName().compare("CA") != 0) {
                eraseKeys.push_back(i->first);
            }
        }

        for(unsigned int i = 0; i < eraseKeys.size(); i++) {
            atoms.erase(atoms.find(eraseKeys[i]));
        }

        eraseKeys.clear();

        list<SerialAndHashType> sortedSerials;
        SerialAndHashType elem;
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            elem.hashKey = i->first;
            elem.serial = i->second.GetSerial();

            sortedSerials.push_back(elem);
        }
        sortedSerials.sort(SerialAndHashTypePredicate());


        list<SerialAndHashType>::iterator oldAtom = sortedSerials.begin();
        list<SerialAndHashType>::iterator startAtom = sortedSerials.begin();

        startAtom++;
        for(list<SerialAndHashType>::iterator i = startAtom; i != sortedSerials.end(); i++) {
            bonds.push_back(PDBBond(oldAtom->hashKey, i->hashKey, false));
            oldAtom = i;
        }
        sortedSerials.clear();
    }

    void CAlphaRenderer::loadSSEHunterFile(string fileName) {
//        RendererBase::load(fileName);
        atoms.clear();
        bonds.clear();
        atoms = PDBReader::ReadAtomPositions(fileName);

        float maxTempFactor = -10000.0f, minTempFactor = 10000.0f;
        float tempFactor;

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            tempFactor = i->second.GetTempFactor();
            if(tempFactor > maxTempFactor) {
                maxTempFactor = tempFactor;
            }
            if(tempFactor < minTempFactor) {
                minTempFactor = tempFactor;
            }
        }
        float r, g, b;

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.SetAtomRadius(3.0);
            tempFactor = i->second.GetTempFactor();
            if(tempFactor < 0) {
                tempFactor = (tempFactor / minTempFactor);
                r = 1.0f - tempFactor;
                g = 1.0f - tempFactor;
                b = 1.0f;
            } else {
                tempFactor = (tempFactor / maxTempFactor);
                r = 1.0f;
                g = 1.0f - tempFactor;
                b = 1.0f - tempFactor;
            }

            i->second.SetColor(r, g, b, 1.0f);
        }
    }
    bool CAlphaRenderer::saveSSEHunterFile(string fileName) {
        return PDBReader::WriteAtomPositions(atoms, fileName);
    }

//      void CAlphaRenderer::GetSSEHunterAtoms(Volume * vol, SkeletonMesh_Annotated * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff) {
//          Renderer::LoadFile("");
//          atoms.clear();
//          bonds.clear();
//
//          SSEHunter * hunter = new SSEHunter();
//          atoms = hunter->GetScoredAtoms(vol, skeleton, resolution, threshold, correlationCoeff, skeletonCoeff, geometryCoeff);
//          delete hunter;
//
//          ColorSSEHunterAtoms();
//      }

    void CAlphaRenderer::updateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff) {
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.SetTempFactor( i->second.GetTotalScore(correlationCoeff, skeletonCoeff, geometryCoeff) );
        }
        colorSSEHunterAtoms();
    }

    void CAlphaRenderer::colorSSEHunterAtoms() {
        float maxTempFactor = -10000.0f, minTempFactor = 10000.0f;
        float tempFactor;

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            tempFactor = i->second.GetTempFactor();
            if(tempFactor > maxTempFactor) {
                maxTempFactor = tempFactor;
            }
            if(tempFactor < minTempFactor) {
                minTempFactor = tempFactor;
            }
        }
        float r, g, b;

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.SetAtomRadius(3.0);
            tempFactor = i->second.GetTempFactor();
            if(tempFactor < 0) {
                tempFactor = (tempFactor / minTempFactor);
                r = 1.0f - tempFactor;
                g = 1.0f - tempFactor;
                b = 1.0f;
            } else {
                tempFactor = (tempFactor / maxTempFactor);
                r = 1.0f;
                g = 1.0f - tempFactor;
                b = 1.0f - tempFactor;
            }

            i->second.SetColor(r, g, b, 1.0f);
        }
    }

    int CAlphaRenderer::selectionObjectCount(){
        int count = selectionAtomCount();
        for(unsigned int i = 0; i < bonds.size(); i++) {
            if(bonds[i].GetSelected()) {
                count++;
            }
        }
        return count;
    }

    int CAlphaRenderer::selectionAtomCount(){
        int count = 0;
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            if(i->second.GetSelected()) {
                count++;
            }
        }
        return count;
    }


    Vec3F CAlphaRenderer::selectionCenterOfMass() {
        int count = 0;
        Vec3F centerOfMass = Vec3F(0,0,0);
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            if(i->second.GetSelected()) {
                count++;
                centerOfMass = centerOfMass + i->second.GetPosition();
            }
        }

        for(unsigned int i = 0; i < bonds.size(); i++) {
            if(bonds[i].GetSelected()) {
                count++;
                centerOfMass = centerOfMass + (atoms[bonds[i].GetAtom0Ix()].GetPosition() + atoms[bonds[i].GetAtom1Ix()].GetPosition()) * 0.5;
            }
        }
        if(count == 0) {
//            centerOfMass = RendererBase::SelectionCenterOfMass();
        } else {
            centerOfMass = centerOfMass * (1.0f/(float)count);
        }
        return centerOfMass;
    }

    bool CAlphaRenderer::selectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
        bool rotated = false;
        Vec3D centerOfMassP3(centerOfMass.X(), centerOfMass.Y(), centerOfMass.Z());
        Vec3D rotationV3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z());

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            if(i->second.GetSelected()) {
                rotated = true;
                Vec3F move = centerOfMass - i->second.GetPosition();
                Vec3D moveV3(move.X(), move.Y(), move.Z());
                Matrix4 rotMatrix = Matrix4::rotation(rotationV3, angle);
                Vec3D newMove = rotMatrix * moveV3;
                newMove = centerOfMassP3 - newMove;
                i->second.SetPosition(Vec3F(newMove[0], newMove[1], newMove[2]));
            }
        }
        return rotated;
    }

    bool CAlphaRenderer::selectionMove(Vec3F moveDirection) {
        bool moved = false;
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            if(i->second.GetSelected()) {
                i->second.SetPosition(i->second.GetPosition() + moveDirection);
                i->second.SetFlag(1);
                moved = true;
            } else {
                i->second.SetFlag(0);
            }
        }

        for(unsigned int i = 0; i < bonds.size(); i++) {
            if(bonds[i].GetSelected()) {
                PDBAtom a = atoms[bonds[i].GetAtom0Ix()];
                if(a.GetFlag() == 0) {
                    a.SetPosition(a.GetPosition() + moveDirection);
                    a.SetFlag(1);
                    moved = true;
                }

                a = atoms[bonds[i].GetAtom1Ix()];
                if(a.GetFlag() == 0) {
                    a.SetPosition(a.GetPosition() + moveDirection);
                    a.SetFlag(1);
                    moved = true;
                }
            }
        }
        return moved;

    }

    void CAlphaRenderer::clearOtherHighlights(){
        selectedSSEHelices.clear();
    }

    void CAlphaRenderer::selectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
//        RendererBase::SelectionToggle(subsceneIndex, forceTrue, ix0, ix1, ix2, ix3, ix4);
        AtomMapType::iterator it;
        PDBAtom * a;
        if (subsceneIndex == 0) {
            //TODO: possibly implement mouse picking using ray intersection
            switch(displayStyle) {
                case CALPHA_DISPLAY_STYLE_BACKBONE:
                case CALPHA_DISPLAY_STYLE_SIDE_CHAIN:
                    it = atoms.find(atomHashKeys.at(ix0));
                    if (it != atoms.end()) {
                        a = &(it->second);
                        a->SetSelected( forceTrue || !a->GetSelected() );
                    }
                    break;
                case CALPHA_DISPLAY_STYLE_RIBBON:

                    if(aHelices[ix0].selected == true && !forceTrue){
                        aHelices[ix0].selected = false;
                    }
                    else{
                        cout << "Updating selectedHelix" << " ix0=" << ix0 << " forceTrue=" << forceTrue << endl;
                        aHelices[ix0].selected = true;
                        selectedHelixIndices.push_back(ix0);
                    }
                    break;
            }
        } else if((subsceneIndex == 1) && (ix0 >= 0) && (ix0 <= (int)bonds.size())) {
            switch(displayStyle) {
                case CALPHA_DISPLAY_STYLE_BACKBONE:
                    bonds[ix0].SetSelected(forceTrue || !bonds[ix0].GetSelected());
                    break;
                case CALPHA_DISPLAY_STYLE_RIBBON:
                    if(bStrands[ix0].selected == true && !forceTrue){
                        bStrands[ix0].selected = false;
                    } else {
                        cout << "Updating selectedStrand ix0=" << ix0 << " forceTrue=" << forceTrue << endl;
                        bStrands[ix0].selected = true;
                        selectedStrandIndices.push_back(ix0);
                    }
                    //cout << "A Ribbon was selected and subscene is 1" << endl;
                    break;
                case CALPHA_DISPLAY_STYLE_SIDE_CHAIN:
                    sidechainBonds[ix0].SetSelected(forceTrue || !sidechainBonds[ix0].GetSelected());
                    break;
            }
        } else if((subsceneIndex == 2) && (ix0 != NULL)) {
            switch(displayStyle) {
                case CALPHA_DISPLAY_STYLE_BACKBONE:
                    break;
                case CALPHA_DISPLAY_STYLE_RIBBON:
                    if(loops[ix0].selected == true && !forceTrue){
                        loops[ix0].selected = false;
                    } else {
                        cout << "Updating selectedStrand ix0=" << ix0 << " forceTrue=" << forceTrue << endl;
                        loops[ix0].selected = true;
                        selectedLoopIndices.push_back(ix0);
                    }
                    //cout << "A Ribbon was selected and subscene is 2" << endl;
                    break;
                case CALPHA_DISPLAY_STYLE_SIDE_CHAIN:
                    break;
            }
        }
        //cout << "Finished updating selected calpha helix" << endl;
        cout << "Finished updating selected secel" << endl;
    }

    void CAlphaRenderer::unload() {
        atoms.clear();
        bonds.clear();
        sidechainBonds.clear();
    }

    string CAlphaRenderer::getSupportedLoadFileFormats() {
        return "Atom Positions (*.pdb)";
    }

    string CAlphaRenderer::getSupportedSaveFileFormats() {
        return "Atom Positions (*.atom)";
    }
    PDBAtom * CAlphaRenderer::getAtom(unsigned long long index) {
        return &atoms[index];
    }

    PDBBond * CAlphaRenderer::getBond(int index) {
        return &bonds[index];
    }

    PDBBond * CAlphaRenderer::getSideChainBond(int index) {
        return &sidechainBonds[index];
    }

    PDBAtom * CAlphaRenderer::getSelectedAtom(unsigned int selectionId) {
        //Python uses this with SelectionAtomCount() to get all the selected atoms
        int count = 0;
        for(AtomMapType::iterator it = atoms.begin(); it != atoms.end(); it++) {
            if(it->second.GetSelected()) {
                if(count == selectionId) {
                    return &it->second;
                }
                count++;
            }
        }
        return NULL;
    }

    vector<unsigned long long> CAlphaRenderer::getAtomHashes() {
        vector<unsigned long long> atomHashes;
        for (AtomMapType::iterator it = atoms.begin(); it != atoms.end(); it++) {
            atomHashes.push_back(it->first);
        }
        return atomHashes;
    }

    int CAlphaRenderer::getBondIndex(unsigned long long atom0, unsigned long long atom1) {
        for(unsigned int i = 0; i < bonds.size(); i++) {
            if(((bonds[i].GetAtom0Ix() == atom0) && (bonds[i].GetAtom1Ix() == atom1)) ||
                ((bonds[i].GetAtom0Ix() == atom1) && (bonds[i].GetAtom1Ix() == atom0))) {
                    return i;
            }
        }
        return -1;
    }

    int CAlphaRenderer::getSideChainBondIndex(unsigned long long atom0, unsigned long long atom1) {
        for(unsigned int i = 0; i < sidechainBonds.size(); i++) {
            if(((sidechainBonds[i].GetAtom0Ix() == atom0) && (sidechainBonds[i].GetAtom1Ix() == atom1)) ||
                ((sidechainBonds[i].GetAtom0Ix() == atom1) && (sidechainBonds[i].GetAtom1Ix() == atom0))) {
                    return i;
            }
        }
        return -1;
    }

    int CAlphaRenderer::getAtomCount() {
        return atoms.size();
    }

    int CAlphaRenderer::getBondCount() {
        return bonds.size();
    }

    int CAlphaRenderer::getSideChainBondCount() {
        return sidechainBonds.size();
    }

    void CAlphaRenderer::deleteAtom(unsigned long long index) {
        atoms.erase(atoms.find(index));
    }

    void CAlphaRenderer::deleteBond(int index) {
        bonds.erase(bonds.begin() + index);
    }

    void CAlphaRenderer::deleteSideChainBond(int index) {
        sidechainBonds.erase(sidechainBonds.begin() + index);
    }

    Vec3F CAlphaRenderer::get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
        Vec3F position;
        switch(subsceneIndex) {
            case(0):
                if((ix0 >= 0) && (ix0 <= (int)atoms.size())) {
                    PDBAtom * a =  & (atoms [ix0]);
                    position = a->GetPosition();
                }
                break;
            case(1):
                if((ix0 >= 0) && (ix0 <= (int)bonds.size())) {
                    position = (atoms[bonds[ix0].GetAtom0Ix()].GetPosition() + atoms[bonds[ix0].GetAtom1Ix()].GetPosition()) * 0.5;
                }
                break;
            default:
                position = Vec3F(0,0,0);
                break;
        }
        return position;
    }

    void CAlphaRenderer::transformAllAtomLocations(MatrixFloat transform) {
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.Transform(transform);
        }
    }

    int CAlphaRenderer::startHelix() {
        aHelices.push_back(Secel());
        return aHelices.size() - 1;
    }

    void CAlphaRenderer::addHelixElement(int index, unsigned long long hashKey){
        aHelices[index].atomHashes.push_back(hashKey);
    }

    int CAlphaRenderer::startStrand() {
        bStrands.push_back(Secel());
        return bStrands.size() - 1;
    }

    void CAlphaRenderer::addStrandElement(int index, unsigned long long hashKey){
        bStrands[index].atomHashes.push_back(hashKey);
    }

    int CAlphaRenderer::startLoop() {
        loops.push_back(Secel());
        return loops.size() - 1;
    }

    void CAlphaRenderer::addLoopElement(int index, unsigned long long hashKey){
        loops[index].atomHashes.push_back(hashKey);
    }

    bool CAlphaRenderer::cleanSecondaryStructures(){
        aHelices.clear();
        bStrands.clear();
        loops.clear();
        return true;
    }

    void CAlphaRenderer::setHelixCorrs(  vector < int > flatCorrespondences){
        if(flatCorrespondences.size() %2 != 0)
            return;
        else
            corrs.clear();
        for(int i=0; i < flatCorrespondences.size(); i = i+2){
            corrs.push_back(make_pair(flatCorrespondences[i], flatCorrespondences[i+1]));
        }
    }

    void CAlphaRenderer::setFeatureVecs(vector<Vec3F> flatFeatureVecs){
        if(flatFeatureVecs.size() %2 != 0)
            return;
        else
            featureVecs.clear();
        for(int i=0; i < flatFeatureVecs.size(); i = i+2){
            featureVecs.push_back(make_pair(flatFeatureVecs[i], flatFeatureVecs[i+1]));
        }

    }
    void CAlphaRenderer::setSelectedSSEHelices(vector<int> indices){
        selectedSSEHelices.clear();
        selectedSSEHelices = indices;
    }

    void CAlphaRenderer::setHelixColor(int helixNum, float r, float g, float b){
        cout << "setting helix color " << helixNum << " to (" << r << ", " << g << ", " << b << ")" <<endl;
        helixColors.erase(helixNum);
        helixColors.insert(make_pair(helixNum, Vec3F(r,g,b)));
    }

    // creates a vector of Vector3DFloats that represents the locations of all the PDBAtoms
    // starting with start and ending with end; it does not error check, so incorrectly
    // ordered points will break this method.  there are more efficient ways to handle this
    // functionality, but this seems simple and flexible enough
    vector<Vec3F> CAlphaRenderer::createPointVector(PDBAtom start, PDBAtom end){
        vector<Vec3F> points;

        PDBAtom current = start;
        while(current.GetHashKey() != end.GetHashKey()){
            points.push_back(current.GetPosition());
            if(current.GetHashKey() == current.GetNextCAHash()){
                break;
            }
            current = atoms.find(current.GetNextCAHash())->second;
        }

        points.push_back(end.GetPosition());
        return points;
    }

    // implementation of Laplacian smoothing for a vector of Vector3DFloats (treats them like points)
    // creating copies of "points" twice seems unnecessary, but I am unsure about the performance cost,
    // so I am leaving it for simplicity of implementation
    vector<Vec3F> CAlphaRenderer::laplacianSmoothing(vector<Vec3F> points, int steps){
        vector<Vec3F> pointsTemp(points);
        vector<Vec3F> smoothedPoints(points);

        for(int i = 0; i < steps; ++i){
            for(int j = 1; j < points.size()-1; ++j){
                smoothedPoints[j] = (pointsTemp[j-1] + pointsTemp[j+1])*.5;
                smoothedPoints[j] = (smoothedPoints[j] + pointsTemp[j])*.5;
            }
            pointsTemp = smoothedPoints;
        }
        return pointsTemp;
    }

    // unsure of what behavior should be if points.size() < 3; in molscript the strand is skipped in this case
    vector<Vec3F> CAlphaRenderer::createStrandNormals(vector<Vec3F> points, Vec3F previous, Vec3F next){
        vector<Vec3F> normals(points);
        int ptsSize = points.size();

        for(int i = 1, length = ptsSize - 1; i < length; ++i){
            Vec3F newPos = (points[i-1] + points[i+1])*.5;
            normals[i] = points[i] - newPos;
            normals[i].normalize();
        }

        normals[0] = (points[1] + previous)*.5 - points[0];
        if ((points[0] - previous).length() < .0001){
            normals[0] = normals[1];
        }

        normals[ptsSize - 1] = (points[ptsSize - 2] + next)*.5 - points[ptsSize - 1];
        if ((points[ptsSize - 2] - next).length() < .0001){
            normals[ptsSize - 1] = normals[ptsSize - 2];
        }

        // "normals must point the same way" - molscript/graphics.c
        for(int j = 0, size = ptsSize - 1; j < size; ++j){
            if(normals[j]*normals[j+1] < 0){
                normals[j+1] = normals[j+1]*-1;
            }
        }

        // "smooth normals, one iteration" - molscript/graphics.c
        vector<Vec3F> smoothedNormals(normals);

        for(int k = 1, size = ptsSize - 1; k < size; ++k){
            smoothedNormals[k] = normals[k-1] + normals[k] + normals[k+1];
            smoothedNormals[k].normalize();
        }

        // "normals exactly perpendicular to strand" - molscript/graphics.c
        Vec3F direction = points[1] - points[0];
        Vec3F side = direction^smoothedNormals[0];
        smoothedNormals[0] = side ^ direction;
        smoothedNormals[0].normalize();

        for(int i = 1, size = ptsSize - 1; i < size; ++i){
            direction = points[i+1] - points[i-1];
            side = direction^smoothedNormals[i];
            smoothedNormals[i] = side^direction;
            smoothedNormals[i].normalize();
        }

        direction = points[ptsSize - 1] - points[ptsSize - 2];
        side = direction^smoothedNormals[ptsSize - 1];
        smoothedNormals[ptsSize - 1] = side^direction;
        smoothedNormals[ptsSize - 1].normalize();
        return smoothedNormals;
    }

    void CAlphaRenderer::createHelixAxesTangentsAndPoints(vector<Vec3F>& axes, vector<Vec3F>& tangents, vector<Vec3F>& interpPoints, std::vector<Vec3F> points, Vec3F previous, Vec3F next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR){
        if(points.size() > 2){

            for(int i = 0; i < points.size() - 1; ++i){

                if(i > 0){
                    Vec3F cvec = points[i+1] - points[i-1];
                    cvec.normalize();

                    Vec3F rvec = (points[i]-points[i-1])^(points[i+1]-points[i]);
                    rvec.normalize();

                    axes[i] = rvec*sin(HELIX_ALPHA) + cvec*cos(HELIX_ALPHA);
                    tangents[i] = rvec*sin(HELIX_BETA) + cvec*cos(HELIX_BETA);
                    tangents[i] = tangents[i]*HELIX_HERMITE_FACTOR;
                }
            }
            axes[0] = axes[1];
            axes[axes.size()-1] = axes[axes.size()-2];

            tangents[0] = previous - points[1];
            tangents[0].normalize();
            tangents[0] = tangents[0]*HELIX_HERMITE_FACTOR;
            tangents[tangents.size()-1] = next - points[points.size()-2];
            tangents[tangents.size()-1].normalize();
            tangents[tangents.size()-1] = tangents[tangents.size()-1]*HELIX_HERMITE_FACTOR;
        }
    }

    // method works like drawing the side of a cylinder with only one stack and 4 slices
    void CAlphaRenderer::drawOpenBox(std::vector<Vec3F> points, std::vector<Vec3F> normals){
        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0, runlength = points.size() + 2; j < runlength; ++j){
            glNormal3f(normals[j%normals.size()].X(), normals[j%normals.size()].Y(), normals[j%normals.size()].Z());
            glVertex3f(points[j%points.size()].X(), points[j%points.size()].Y(), points[j%points.size()].Z());
        }

        glEnd();
    }



    // renders a set of points and normals assuming that they are laid out like the side of a cylinder's points and normals
    void CAlphaRenderer::drawTube(std::vector<Vec3F> points, std::vector<Vec3F> normals, int stacks, int slices){
        //glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
        //glDisable(GL_CULL_FACE);
        switch (renderingType){
            case 0:
                glColorMaterial ( GL_FRONT_AND_BACK, GL_SPECULAR | GL_AMBIENT_AND_DIFFUSE );
                glEnable ( GL_COLOR_MATERIAL );
                break;
            default:
                break;
        }
        for(int i = 0, runlength = points.size(); i < runlength; ++i){
            if (i%(stacks+1) == 0){
                glBegin(GL_TRIANGLE_STRIP);
            }

            int nextSliceIx = (i+stacks+1)%runlength;

            glNormal3f(normals[i].X(), normals[i].Y(), normals[i].Z());
            glVertex3f(points[i].X(), points[i].Y(), points[i].Z());

            glNormal3f(normals[nextSliceIx].X(), normals[nextSliceIx].Y(), normals[nextSliceIx].Z());
            glVertex3f(points[nextSliceIx].X(), points[nextSliceIx].Y(), points[nextSliceIx].Z());

            if((i+1)%(stacks+1) == 0){
                glEnd();
            }
        }
    }

    vector<Vec3F> CAlphaRenderer::interpolateLoopPoints(std::vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS){
        HermiteCurve curve;
        Vec3F m0, m1;
        vector<Vec3F> pointstemp(points);
        bool LAPLACIAN_SMOOTHING = true;
        int SMOOTHING_STEPS = 1;
        double HERMITE_FACTOR = 0.5;
        int LOOP_SLICES = 10;
        if(LAPLACIAN_SMOOTHING){
            pointstemp = laplacianSmoothing(points, SMOOTHING_STEPS);
        }

        vector<Vec3F> interpolatedPoints((pointstemp.size()-1)*(NUM_SEGMENTS));

        for(unsigned int i = 0; i < points.size()-1; ++i){
            if(i == 0){
                m0 = pointstemp[i+1] - previous;
            } else {
                m0 = pointstemp[i+1] - pointstemp[i-1];
                m0 = m0*HERMITE_FACTOR;
            }

            if(i + 2 > pointstemp.size() - 1){
                m1 = next - pointstemp[i];
            } else {
                m1 = pointstemp[i+2] - pointstemp[i];
                m1 = m1*HERMITE_FACTOR;
            }

            curve.setCurve(pointstemp[i], pointstemp[i+1], m0, m1);
            interpolatedPoints[i*(NUM_SEGMENTS)] = pointstemp[i];
            for (int sect = 1; sect < NUM_SEGMENTS; ++sect){
                double tsect = ((double)sect)/((double)NUM_SEGMENTS);
                interpolatedPoints[i*(NUM_SEGMENTS) + sect] = curve.getPos(tsect);
            }
        }
        interpolatedPoints[interpolatedPoints.size()-1] = points[points.size() -1];
        return interpolatedPoints;
    }

    void CAlphaRenderer::setNumSegments(int segments){
        NUM_SEGMENTS = segments;
    }

    void CAlphaRenderer::setNumSlices(int slices){
        NUM_SLICES = slices;
    }

} /* namespace Visualization */
