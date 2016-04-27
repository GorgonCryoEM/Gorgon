/*
 * NonManifoldMesh.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "NonManifoldMesh.h"

namespace Protein_Morph {

    Edge::Edge()
            : ids(2)
    {}

    Edge::Edge(TKey v1, TKey v2)
            : ids(2)
    {
        ids[0] = v1;
        ids[1] = v2;
    }

    vector<TKey> Edge::getVertices() const {
        return ids;
    }

    CKey Edge::getVerticesSet() const {
        return CKey(ids.begin(), ids.end());
    }

    TKey Edge::vertex(int i) const {
        return ids[i];
    }

    bool operator<(const Edge &l, const Edge &r) {
        vector<TKey> ll = l.ids;
        vector<TKey> rr = r.ids;

        if(ll.size() != 2 && rr.size() != 2)
            throw "Edges have inconsistent size!";

        sort(ll.begin(), ll.end());
        sort(rr.begin(), rr.end());

        return ll[0]<rr[0] && ll[1]<rr[1];
    }

    ostream& operator<<(ostream& out, const Edge& obj){
//        set<unsigned int> faces(obj.faceIds.begin(), obj.faceIds.end());
        return out//<<"\033[34m"
                  <<"\tvertexIds: "<<obj.ids[0]<<"\t"<<obj.ids[1]<<endl
//                  <<"faceIds.size(): "<<obj.faceIds.size()
//                  <<faces
                  <<endl;
//                  <<"\033[0m";
    }

    CElem Face::getVertices() const {
        return CElem(ids.begin(), ids.end());
    }

    void Face::addVertex(TKey i) {
        ids.push_back(i);
    }

    ostream& operator<<(ostream& out, const Face& obj) {
        set<unsigned int> vertices(obj.ids.begin(), obj.ids.end());
//        set<unsigned int> edges(obj.edgeIds.begin(), obj.edgeIds.end());
        return out //<<"\033[34m"
//                    << "\nedgeIds.size(): " << obj.edgeIds.size() << endl
//                    << edges
                    << "\n\nvertexIds.size(): " << obj.ids.size()
                    << endl
                    << vertices << endl << endl;
//                    <<"\033[0m";
    }

    NonManifoldMesh::NonManifoldMesh()
            : fromVolume(false)
    {
        setSpacing(1, 1, 1);
        setOrigin(0, 0, 0);
    }

    NonManifoldMesh::NonManifoldMesh(const Volume & src) {
#ifdef GORGON_DEBUG
        cout<<"\033[33mDEBUG: File:   NonManifoldMesh.h"<<endl;
        cout<<"DEBUG: Method: NonManifoldMesh::NonManifoldMesh\033[0m"<<endl;
        cout<<"DEBUG: Args: Volume*\033[0m"<<endl;
        cout<<"src.getSize(): "<<src.getSize()<<endl;
#endif

        int lVertices[src.getSize()];

        fromVolume = true;
        size = src.getSizeObj();
        setOrigin(src.getOriginObj());
        setSpacing(src.getSpacingObj());

        // Adding vertices
        for(int x = 0; x < src.getSizeX(); x++) {
            for(int y = 0; y < src.getSizeY(); y++) {
                for(int z = 0; z < src.getSizeZ(); z++) {
                    Vec3I pos(x, y, z);
                    int i = src.getIndex(pos);
                    lVertices[i] = -1;
                    int value = (int)round(src(i));
                    if(value > 0)
                        lVertices[i] = addVertex(Vec3F(x, y, z));
                }
            }
        }

        //Adding edges
        int ens[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
        for(int x = 0; x < src.getSizeX()-1; x++) {
            for(int y = 0; y < src.getSizeY()-1; y++) {
                for(int z = 0; z < src.getSizeZ()-1; z++) {
                    Vec3I pos(x, y, z);
                    int i = src.getIndex(pos);
                    for(int k = 0; k < 3; k++) {
                        int i2 = src.getIndex(pos+ens[k]);
                        if((lVertices[i] >= 0) && (lVertices[i2] >= 0)) {
                            addEdge(lVertices[i], lVertices[i2]);
                        }
                    }
                }
            }
        }

        //Adding Faces
        int fns[3][3][3] = {{{1,0,0}, {1,0,1}, {0,0,1}},
                            {{1,0,0}, {1,1,0}, {0,1,0}},
                            {{0,1,0}, {0,1,1}, {0,0,1}}};
        int inds[4];
        for(int x = 0; x < src.getSizeX()-1; x++) {
            for(int y = 0; y < src.getSizeY()-1; y++) {
                for(int z = 0; z < src.getSizeZ()-1; z++) {
                    Vec3I pos(x, y, z);
                    int ind = src.getIndex(pos);
                    if(lVertices[ind] >= 0) {
                        for(int i = 0; i < 3; i++) {
                            bool faceFound = true;
                            inds[0] = lVertices[ind];
                            for(int j = 0; j < 3; j++) {
                                int ind2 = src.getIndex(pos+fns[i][j]);
                                inds[j+1] = lVertices[ind2];
                                faceFound = faceFound && lVertices[ind2] >= 0;
                            }
                            if(faceFound) {
                                addQuad(inds[0], inds[1], inds[2], inds[3]);
                            }
                        }
                    }
                }
            }
        }

#ifdef GORGON_DEBUG
        cout<<"\033[33mDEBUG: END"<<endl;
        cout<<"DEBUG: Method: NonManifoldMesh::NonManifoldMesh\n\033[0m"<<endl;
#endif

    }

    void NonManifoldMesh::clear() {
        Mesh::clear();
        vertices.clear();
        curves.clear();
        faces.clear();
    }

    int NonManifoldMesh::addMarchingVertex(Vec3F location) {
        return addVertex(location);
    }

    TKey NonManifoldMesh::addMarchingFace(Vec3U vertexHash) {
        return addTriangle(vertexHash);
    }

    void NonManifoldMesh::draw(bool drawSurfaceBorders, bool drawSurfaces,
                               bool drawLines, bool drawPoints,
                               bool annotateSurfaces, bool annotateLines, bool annotatePoints,
                               bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting,
                               int lineThickness, bool smoothSurfaceNormals)
    {
        ofstream fout1("vertices.txt");
        ofstream fout2("faces.txt");
        ofstream fout3("edges.txt");
        ofstream fout4("non-manifold-mesh.txt");

        map<unsigned int, Vertex> mapVertices;
        for(unsigned int i=0; i<vertices.size(); ++i)
            mapVertices[i] = vertices[i];

        map<unsigned int, Edge> mapEdges;
        for(unsigned int i=0; i<curves.size(); ++i)
            mapEdges[i] = curves[i];

        map<unsigned int, Face> mapFaces;
        for(unsigned int i=0; i<faces.size(); ++i)
            mapFaces[i] = faces[i];

        fout1 << mapVertices;
        fout2 << mapFaces;
        fout3 << mapEdges;
        fout4 << *this;

        glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT | GL_POINT_BIT);

        if(drawSurfaceBorders) {
            glLineWidth(1);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

            if(disableCurveLighting) {
                glDisable(GL_LIGHTING);
            }
            for(unsigned int i = 0; i < faces.size(); i++) {
                glColor4f(0.4, 0.7, 0.7, 0.6);
                glBegin(GL_LINE_STRIP);

                CElem v = faces[i].getVertices();
                for(CElem::iterator it=v.begin(); it!=v.end(); ++it) {
                    int k = *it;
                    float vals[3];
                    vals[0] = vertices[k][0];
                    vals[1] = vertices[k][1];
                    vals[2] = vertices[k][2];
                    glVertex3fv(vals);
                }
                glEnd();
            }
        }

        if(drawSurfaces) {
            if(disableSurfaceLighting) {
                glDisable(GL_LIGHTING);
            }
            if(annotateSurfaces) {
                glPushName(0);
                glPushName(0);
            }
//            #ifdef GORGON_DEBUG
                  cout<<"\033[32m"<<endl;
                  cout<<"DEBUG: Method: NonManifoldMesh::draw(bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, int, bool)\033[0m"<<endl;
                  cout<<"faces.size(): "<<faces.size()<<endl;
                  cout<<"\033[0m";
//            #endif

            for(unsigned int i = 0; i < faces.size(); i++) {
                if(annotateSurfaces) {
                    glLoadName(i);
                }
                glColor4f(1.2, 0.2, 0.2, 0.6);
                glBegin(GL_POLYGON);

                CElem v = faces[i].getVertices();
                for(CElem::iterator it=v.begin(); it!=v.end(); ++it) {
                    Vec3F normal;
                    if(smoothSurfaceNormals) {
                        normal = getVertexNormal(*it);
                    } else {
                        normal = getFaceNormal(i);
                    }
                    int k = *it;
                    glNormal3f(normal.X(), normal.Y(), normal.Z());
                    float vals[3];
                    vals[0] = vertices[k][0];
                    vals[1] = vertices[k][1];
                    vals[2] = vertices[k][2];
                    glVertex3fv(vals);
                }
                glEnd();
            }
            if(annotateSurfaces) {
                glPopName();
                glPopName();
            }
        }

        if(drawLines) {
            if(disableCurveLighting) {
                glDisable(GL_LIGHTING);
            }
            if(annotateLines) {
                glPushName(1);
                glPushName(0);
            }
            glLineWidth(lineThickness);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            for(unsigned int i = 0; i < curves.size(); i++) {
//                if(edges[i].size() == 0) {
                    if(annotateLines) {
                        glLoadName(i);
                    }
                    glBegin(GL_LINES);
                    int k = curves[i].vertex(0);
                    glVertex3f(vertices[k][0], vertices[k][1], vertices[k][2]);
                    k = curves[i].vertex(1);
                    glVertex3f(vertices[k][0], vertices[k][1], vertices[k][2]);
                    glEnd();
//                }
            }
            if(annotateLines) {
                glPopName();
                glPopName();
            }
        }

        if(drawPoints) {
            if(disablePointLighting) {
                glDisable(GL_LIGHTING);
            }

            if(annotatePoints) {
                glPushName(2);
                glPushName(0);
            }
            glPointSize(2);
            glEnable(GL_POINT_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            for(unsigned int i = 0; i < vertices.size(); i++) {
                if(vertices[i].sizeEdge() == 0) {
                    if(annotatePoints) {
                        glLoadName(i);
                    }
                    glBegin(GL_POINTS);
                    glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
                    glEnd();
                }
            }
            if(annotatePoints) {
                glPopName();
                glPopName();
            }
        }
        glPopAttrib();

        glFlush();
    }

    int NonManifoldMesh::addVertex(Vec3F location) {
        TKey id = vertices.size();
        vertices[id] = Vertex(location);

        return id;
    }

    int NonManifoldMesh::addEdge(Edge edge) {
        TKey id = curves.size();
        curves[id] = edge;

        return id;
    }

    Edge NonManifoldMesh::addEdge(int v1, int v2) {
        Edge edge(v1, v2);
        int id = addEdge(edge);
        vertices[v1].addFaceHash(id);
        vertices[v2].addFaceHash(id);

        return edge;
    }


    int NonManifoldMesh::addFace(Face face) {
        TKey id = faces.size();
        faces[id] = face;

        return id;
    }

    TKey NonManifoldMesh::addTriangle(Vec3U vertex) {
        int v0 = vertex[0];
        int v1 = vertex[1];
        int v2 = vertex[2];

//        add to faces: vertex Ids
        Face face;
        face.addVertex(v0);
        face.addVertex(v1);
        face.addVertex(v2);

//        add to edges: face IDs
        int faceId = addFace(face);

        return faceId;
    }

    void NonManifoldMesh::addQuad(int v1, int v2, int v3, int v4) {
        addTriangle(Vec3U(v1, v2, v3));
        addTriangle(Vec3U(v1, v3, v4));
    }

    Vec3F NonManifoldMesh::getVertexNormal(int id) {
        Vec3F normal(0, 0, 0);
        Vertex vertex = vertices[id];
        CElem fs = vertex.getFaceHashes();

        for(CElem::iterator it=fs.begin(); it!=fs.end(); ++it)
                normal += getFaceNormal(*it);

        normal.normalize();
        return normal;
    }

    Vec3F NonManifoldMesh::getFaceNormal(int id) {
        Face face = faces[id];
        Vec3F normal(1, 0, 0);

        CElem v = face.getVertices();
        if(v.size() >= 3) {
            normal = (vertices[v[1]] - vertices[v[0]])
                    ^ (vertices[v[2]] - vertices[v[0]]);
            normal.normalize();
        }
        return normal;
    }

    vector<TKey> NonManifoldMesh::getPath(TKey id0, TKey id1) {
        vector<TKey> path;
        map<TKey, TKey> src;

        queue<TKey> q;
        q.push(id0);

        bool found = false;

        while((q.size() > 0) && !found) {
            TKey currentEdge = q.front();
            q.pop();
            found = currentEdge == id1;
            if(!found) {
                for(unsigned int v = 0; v < 2; v++) {
                    TKey i = curves[currentEdge].vertex(v);
                    for(unsigned int e = 0; e < vertices[i].sizeEdge(); e++) {
                        unsigned int id = vertices[i].edge(e);
                        if(src.find(id) == src.end()) {
                            src[id] = currentEdge;
                            q.push(id);
                        }
                    }
                }
            }
        }

        if(found) {
            TKey currentEdge = id1;
            path.push_back(currentEdge);
            while(currentEdge != id0) {
                currentEdge = src[currentEdge];
                path.push_back(currentEdge);
            }
        }

        return path;
    }

    vector<Vec3F> NonManifoldMesh::sampleTriangle(int id, double step) {
        Face face = faces[id];

        vector<Vec3F> points;
        CElem v = face.getVertices();
        if(v.size() != 3) {
            printf("ERROR: Sampling a polygon NOT a triangle!\n");
            return points;
        } else {
            Vertex p = vertices[v[0]];
            Vertex q = vertices[v[1]];
            Vertex r = vertices[v[2]];
            Vec3F v1 = q - p;
            Vec3F v2 = r - p;
            double v1Length = v1.length();
            double v2Length = v2.length();
            v1.normalize();
            v2.normalize();

            for(double a1 = 0; a1 <= v1Length; a1 += step) {
                for(double a2 = 0; a2 <= v2Length; a2 += step) {
                    if(a1/v1Length + a2/v2Length <= 1) {
                        points.push_back(p + v1 * a1 + v2 * a2);
                    }
                }
            }
            return points;
        }
    }

    int NonManifoldMesh::getClosestVertexIndex(Vec3F pos) {
        if(vertices.size() == 0) {
            return -1;
        }

        double minDist = (pos - vertices[0]).length();
        int id = 0;
        for(unsigned int i = 0; i < vertices.size(); i++) {
            double d = (pos - vertices[i]).length();
            if(d < minDist) {
                minDist = d;
                id = i;
            }

        }
        return id;
    }

    bool NonManifoldMesh::isSurfaceVertex(int id) const {
        Vertex vertex;
        TV::const_iterator it = vertices.find(id);
        if(it!=vertices.end())
            vertex = it->second;

        return vertex.sizeFaceHashes() > 0;
    }

    vector<TKey> NonManifoldMesh::getNeighboringVertexIndices(TKey id) {
        vector<TKey> neighbors;
        for(unsigned int i = 0; i < vertices[id].sizeEdge(); i++) {
            if(curves[vertices[id].edge(i)].vertex(0) == id) {
                neighbors.push_back(curves[vertices[id].edge(i)].vertex(1));
            } else {
                neighbors.push_back(curves[vertices[id].edge(i)].vertex(0));
            }
        }
        return neighbors;
    }

    Volume NonManifoldMesh::toVolume() {
        double minPos[3] = {MAX_DOUBLE, MAX_DOUBLE, MAX_DOUBLE};
        double maxPos[3] = {MIN_DOUBLE, MIN_DOUBLE, MIN_DOUBLE};
        if(fromVolume) {
            minPos[0] = 0;
            minPos[1] = 0;
            minPos[2] = 0;
            maxPos[0] = getSizeX() - 1;
            maxPos[1] = getSizeY() - 1;
            maxPos[2] = getSizeZ() - 1;
        }

        for(unsigned int i = 0; i < vertices.size(); i++) {
            for(unsigned int j = 0; j < 3; j++) {
                minPos[j] = min(minPos[j], (double)vertices[i][j]);
                maxPos[j] = max(maxPos[j], (double)vertices[i][j]);
            }
        }

        Vec3I minPosInt;
        Vec3I maxPosInt;

        for(unsigned int j = 0; j < 3; j++) {
            minPosInt[j] = (int)floor(minPos[j]);
            maxPosInt[j] = (int)ceil(maxPos[j]);
        }

        Volume vol(maxPosInt[0] - minPosInt[0]+1, maxPosInt[1] - minPosInt[1]+1, maxPosInt[2] - minPosInt[2]+1);

        for(unsigned int i = 0;  i < curves.size(); i++) {
            Vertex v1 = vertices[curves[i].vertex(0)];
            Vertex v2 = vertices[curves[i].vertex(1)];
            vector<Vec3I> positions = Rasterizer::ScanConvertLineC8(v1.XInt(), v1.YInt(), v1.ZInt(), v2.XInt(), v2.YInt(), v2.ZInt());
            for(unsigned int j = 0; j < positions.size(); j++) {
                vol(positions[j] - minPosInt) = 1.0;
            }
        }

        vol.setOrigin(origin);
        vol.setSpacing(spacing);
        return vol;
    }

    NonManifoldMesh NonManifoldMesh::loadOffFile(string fileName) {
        ifstream inFile(fileName.c_str());
        string strTemp;
        int nVertices, nEdges, nFaces;

        inFile >> strTemp;
        //printf("[%s]\n", strTemp);
        inFile >> nVertices >> nFaces >> nEdges;
        //printf("[%d] [%d] [%d]\n", nVertices, nFaces, nEdges);

        NonManifoldMesh mesh;
        for(int i=0, lVertices=0; i < nVertices; i++, lVertices++) {
            float xPos, yPos, zPos;
            inFile >> xPos >> yPos >> zPos;
            //printf("[%f] [%f] [%f]\n", xPos, yPos, zPos);
            mesh.addVertex(Vec3F(xPos, yPos, zPos));
            inFile >> strTemp;
        }

        int faceNodes[100], nFaceNodes;
        int lFaces = 0;
        for(int i=0; i < nFaces; i++) {
            inFile>>nFaceNodes;
            //printf("[%d]\n", nFaceNodes);
            switch(nFaceNodes) {
                case 1:
                case 2:
                    printf("Cannot load polygon... unsupported polygon size: %d\n", nFaceNodes);
                    break;
                case 4:
                    lFaces++;
                    for(int i = 0; i < nFaceNodes; i++) {
                        //                        fscanf(inFile, "");
                        inFile >> faceNodes[i];
                    }

                    if((faceNodes[0] != faceNodes[1]) && (faceNodes[0] != faceNodes[2]) && (faceNodes[0] != faceNodes[3])
                            && (faceNodes[1] != faceNodes[2]) && (faceNodes[1] != faceNodes[3]) && (faceNodes[2] != faceNodes[3])) {
                        mesh.addQuad(faceNodes[0], faceNodes[1], faceNodes[2], faceNodes[3]);
                    } else {
                        mesh.addEdge(faceNodes[0], faceNodes[2]);
                    }
                    break;
                default:
                    lFaces++;
                    for(int i = 0; i < nFaceNodes; i++) {
                        //                        fscanf(inFile, "");
                        inFile >> faceNodes[i];
                    }
                    for(int i = 2; i < nFaceNodes; i++) {
                        mesh.addTriangle(Vec3U(faceNodes[0], faceNodes[i-1], faceNodes[i]));
                    }
                    break;

            }
            inFile >> strTemp;
        }

        //printf(" Vertices %d of %d loaded.  Faces %d of %d loaded", lVertices, nVertices, lFaces, nFaces);

        inFile.close();
        return mesh;
    }

    ostream& operator<<(ostream& out, const NonManifoldMesh& obj) {
        return out //<<"\033[34m"
               << "vertices.size(): "
               << obj.vertices.size()
               << "\nedges.size(): " << obj.curves.size()
               << "\nfaces.size(): "
               << obj.faces.size() << endl;
//                      <<"\033[0m";
    }

}
