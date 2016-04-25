/*
 * NonManifoldMesh.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "NonManifoldMesh.h"

namespace Protein_Morph {

    NonManifoldMesh::NonManifoldMesh()
            : fromVolume(false)
    {
        setSpacing(1, 1, 1);
        setOrigin(0,0,0);
    }

    NonManifoldMesh::NonManifoldMesh(const Volume & src) {
    #ifdef GORGON_DEBUG
    cout<<"\033[33mDEBUG: File:   NonManifoldMesh.h"<<endl;
    cout<<"DEBUG: Method: NonManifoldMesh::NonManifoldMesh\033[0m"<<endl;
    cout<<"DEBUG: Args: Volume*\033[0m"<<endl;
    cout<<"src.getSize(): "<<src.getSize()<<endl;
    #endif

        int vertexLocations[src.getSize()];

        fromVolume = true;
        size = src.getSizeObj();
        setOrigin(src.getOriginX(), src.getOriginY(), src.getOriginZ());
        setSpacing(src.getSpacingX(), src.getSpacingY(), src.getSpacingZ());

    // Adding vertices
        for(int x = 0; x < src.getSizeX(); x++) {
            for(int y = 0; y < src.getSizeY(); y++) {
                for(int z = 0; z < src.getSizeZ(); z++) {
                    int index = src.getIndex(x, y, z);
                    vertexLocations[index] = -1;
                    int value = (int)round(src(index));
                    if(value > 0) {
                        Vertex tempVertex(Vec3F(x, y, z));
                        vertexLocations[index] = addVertex(tempVertex);
                    }
                }
            }
        }

        //Adding edges
        int edgeNeighbors[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
        for(int x = 0; x < src.getSizeX()-1; x++) {
            for(int y = 0; y < src.getSizeY()-1; y++) {
                for(int z = 0; z < src.getSizeZ()-1; z++) {
                    int index = src.getIndex(x, y, z);
                    for(int i = 0; i < 3; i++) {
                        int index2 = src.getIndex(x+edgeNeighbors[i][0], y+edgeNeighbors[i][1], z+edgeNeighbors[i][2]);
                        if((vertexLocations[index] >= 0) && (vertexLocations[index2] >= 0)) {
                            addEdge(vertexLocations[index], vertexLocations[index2]);
                        }
                    }
                }
            }
        }

        //Adding Faces
        int faceNeighbors[3][3][3] = {  {{1,0,0}, {1,0,1}, {0,0,1}},
                                        {{1,0,0}, {1,1,0}, {0,1,0}},
                                        {{0,1,0}, {0,1,1}, {0,0,1}}};
        int indices[4];
        for(int x = 0; x < src.getSizeX()-1; x++) {
            for(int y = 0; y < src.getSizeY()-1; y++) {
                for(int z = 0; z < src.getSizeZ()-1; z++) {
                    int index = src.getIndex(x, y, z);
                    if(vertexLocations[index] >= 0) {
                        for(int i = 0; i < 3; i++) {
                            bool faceFound = true;
                            indices[0] = vertexLocations[index];
                            for(int j = 0; j < 3; j++) {
                                int index2 = src.getIndex(x+faceNeighbors[i][j][0], y+faceNeighbors[i][j][1], z+faceNeighbors[i][j][2]);
                                indices[j+1] = vertexLocations[index2];
                                faceFound = faceFound && vertexLocations[index2] >= 0;
                            }
                            if(faceFound) {
                                addQuad(indices[0], indices[1], indices[2], indices[3]);
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
        edges.clear();
        faces.clear();
        vertexHashMap.clear();
    }

    int NonManifoldMesh::addMarchingVertex(Vec3F location, int hashKey){
        HashMapType::const_iterator pos = vertexHashMap.find(hashKey);
        int vertexId;
        if(pos == vertexHashMap.end()) {
            vertexId = addVertex(location);
            vertexHashMap[hashKey] = vertexId;
        } else {
            vertexId = pos->second;
        }
        return vertexId;
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

                vector<TKey> v = faces[i].getVerticesVec();
                for(unsigned int j = 0; j < v.size(); j++) {
                    int k = faces[i].vertex(j);
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

                  ofstream fout1("vertices.txt");
                  ofstream fout2("faces.txt");
                  ofstream fout3("edges.txt");
                  ofstream fout4("non-manifold-mesh.txt");

                  map<unsigned int, Vertex> mapVertices;
                  for(unsigned int i=0; i<vertices.size(); ++i)
                      mapVertices[i] = vertices[i];

                  map<unsigned int, Edge> mapEdges;
                  for(unsigned int i=0; i<edges.size(); ++i)
                      mapEdges[i] = edges[i];

                  map<unsigned int, Face> mapFaces;
                  for(unsigned int i=0; i<faces.size(); ++i)
                      mapFaces[i] = faces[i];

                  fout1<<mapVertices;
                  fout2<<mapFaces;
                  fout3<<mapEdges;
                  fout4<<*this;

//            #endif

            for(unsigned int i = 0; i < faces.size(); i++) {
                if(annotateSurfaces) {
                    glLoadName(i);
                }
                glColor4f(1.2, 0.2, 0.2, 0.6);
                glBegin(GL_POLYGON);

                vector<TKey> v = faces[i].getVerticesVec();
                for(unsigned int j = 0; j < v.size(); j++) {
                    Vec3F normal;
                    if(smoothSurfaceNormals) {
                        normal = getVertexNormal(faces[i].vertex(j));
                    } else {
                        normal = getFaceNormal(i);
                    }
                    int k = faces[i].vertex(j);
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
            for(unsigned int i = 0; i < edges.size(); i++) {
                if(edges[i].size() == 0) {
                    if(annotateLines) {
                        glLoadName(i);
                    }
                    glBegin(GL_LINES);
                    int k = edges[i].vertex(0);
                    glVertex3f(vertices[k][0], vertices[k][1], vertices[k][2]);
                    k = edges[i].vertex(1);
                    glVertex3f(vertices[k][0], vertices[k][1], vertices[k][2]);
                    glEnd();
                }
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
        Vertex vertex(location);

        TKey id = vertices.size();
        vertices.push_back(vertex);

        return id;
    }

    int NonManifoldMesh::addEdge(Edge edge) {
        TKey id = edges.size();
        edges.push_back(edge);

        return id;
    }

    Edge NonManifoldMesh::addEdge(int vertexId1, int vertexId2) {
        Edge edge(vertexId1, vertexId2);
        int edgeId = addEdge(edge);
        vertices[vertexId1].addFaceHash(edgeId);
        vertices[vertexId2].addFaceHash(edgeId);

        return edge;
    }

    bool NonManifoldMesh::isEdgePresent(int vertexId1, int vertexId2) {
        bool isPresent = false;
        for(unsigned int i = 0; (i < vertices[vertexId1].sizeEdge()) && !isPresent; i++) {
            isPresent = ((int)edges[vertices[vertexId1].edge(i)].vertex(0) == vertexId2) || ((int)edges[vertices[vertexId1].edge(i)].vertex(1) == vertexId2);
        }

        return isPresent;
    }

    int NonManifoldMesh::getEdgeIndex(int vertexId1, int vertexId2) const {
        int edgeId = -1;
        for(int i = 0; i < vertices[vertexId1].sizeEdge(); i++) {
            if((edges[vertices[vertexId1].edge(i)].vertex(0) == vertexId2) ||
                    (edges[vertices[vertexId1].edge(i)].vertex(1) == vertexId2)) {
                edgeId = vertices[vertexId1].edge(i);
            }
        }
        return edgeId;
    }

    int NonManifoldMesh::addFace(Face face) {
        TKey id = faces.size();
        faces.push_back(face);

        return id;
    }

    TKey NonManifoldMesh::addTriangle(Vec3U vertexId) {
        int vertexId1 = vertexId[0];
        int vertexId2 = vertexId[1];
        int vertexId3 = vertexId[2];

//        add to edges: edge
//        add to vertices: edge IDs
//        add to edges: vertex IDs
        if(!isEdgePresent(vertexId1, vertexId2))
            addEdge(vertexId1, vertexId2);

        if(!isEdgePresent(vertexId2, vertexId3))
            addEdge(vertexId2, vertexId3);

        if(!isEdgePresent(vertexId3, vertexId1))
            addEdge(vertexId3, vertexId1);

//        add to faces: vertex Ids
        Face face;
        face.addVertex(vertexId1);
        face.addVertex(vertexId2);
        face.addVertex(vertexId3);

        int vertexIds[4] = {vertexId1, vertexId2, vertexId3, vertexId1};

//        add to faces: edge Ids
        for(int i = 0; i < 3; i++) {
            int vertexIndex = vertexIds[i];
            for (int j = 0; j < (int)vertices[vertexIndex].sizeEdge(); j++) {
                int edgeIndex = vertices[vertexIndex].edge(j);

                if( (((int)edges[edgeIndex].vertex(0) == vertexIds[i])   && ((int)edges[edgeIndex].vertex(1) == vertexIds[i+1])) ||
                        (((int)edges[edgeIndex].vertex(0) == vertexIds[i+1]) && ((int)edges[edgeIndex].vertex(1) == vertexIds[i]))) {
                    face.addEdge(vertices[vertexIndex].edge(j));
                }
            }
        }

//        add to edges: face IDs
        int faceId = addFace(face);
        for(int i = 0; i < (int)face.size(); i++)
            edges[face.edge(i)].addEdge(faceId);

        return faceId;
    }

    void NonManifoldMesh::addQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4) {
        Vec3U v123(vertexId1, vertexId2, vertexId3);
        addTriangle(v123);

        Vec3U v134(vertexId1, vertexId3, vertexId4);
        addTriangle(v134);
    }

    void NonManifoldMesh::mergeMesh(const NonManifoldMesh & src) {
        vector<int> indices;

        for(unsigned int i = 0; i < src.vertices.size(); i++)
            indices.push_back(addVertex(src.vertices[i]));

        for(unsigned int i = 0; i < src.edges.size(); i++)
            addEdge(indices[src.edges[i].vertex(0)], indices[src.edges[i].vertex(1)]);

        for(unsigned int i = 0; i < src.faces.size(); i++) {
            vector<TKey> v = src.faces[i].getVerticesVec();
            if(v.size() == 3) {
                Vec3U temp(indices[v[0]],
                           indices[v[1]],
                           indices[v[2]]);
                addTriangle(temp);
            }
            else if(v.size() == 3)
                addQuad(indices[v[0]],
                        indices[v[1]],
                        indices[v[2]],
                        indices[v[3]]);
        }
    }

    Vec3F NonManifoldMesh::getVertexNormal(int vertexId) {
        Vec3F normal(0,0,0);
        for(unsigned int i = 0; i < vertices[vertexId].sizeEdge(); i++) {
            int edgeIndex = vertices[vertexId].edge(i);
            CKey v = edges[edgeIndex].getFaces();
            for(unsigned int j = 0; j < v.size(); j++) {
                normal += getFaceNormal(edges[edgeIndex].edge(j));
            }
        }
        normal.normalize();
        return normal;
    }

    Vec3F NonManifoldMesh::getFaceNormal(int faceId) {
        Face face = faces[faceId];
        Vec3F normal(1,0,0);

        vector<TKey> v = face.getVerticesVec();
        if(v.size() >= 3) {
            normal = (vertices[v[1]] - vertices[v[0]]) ^
                     (vertices[v[2]] - vertices[v[0]]);
            normal.normalize();
        }
        return normal;
    }

    vector<TKey> NonManifoldMesh::getPath(TKey edge0Ix, TKey edge1Ix) {
        vector<TKey> path;
        map<TKey,  TKey> source;

        queue<TKey> edgeList;
        edgeList.push(edge0Ix);

        bool found = false;

        while((edgeList.size() > 0) && !found) {
            TKey currentEdge = edgeList.front();
            edgeList.pop();
            found = currentEdge == edge1Ix;
            if(!found) {
                for(unsigned int v = 0; v < 2; v++) {
                    TKey vertexIx = edges[currentEdge].vertex(v);
                    for(unsigned int e = 0; e < vertices[vertexIx].sizeEdge(); e++) {
                        unsigned int edgeIx = vertices[vertexIx].edge(e);
                        if(source.find(edgeIx) == source.end()) {
                            source[edgeIx] = currentEdge;
                            edgeList.push(edgeIx);
                        }
                    }
                }
            }
        }

        if(found) {
            TKey currentEdge = edge1Ix;
            path.push_back(currentEdge);
            while(currentEdge != edge0Ix) {
                currentEdge = source[currentEdge];
                path.push_back(currentEdge);
            }
        }

        return path;
    }

    vector<Vec3F> NonManifoldMesh::sampleTriangle(int faceId, double discretizationStep) {
        Face face = faces[faceId];

        vector<Vec3F> points;
        vector<TKey> v = face.getVerticesVec();
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

            for(double a1 = 0; a1 <= v1Length; a1 += discretizationStep) {
                for(double a2 = 0; a2 <= v2Length; a2 += discretizationStep) {
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

        double minDistance = (pos - vertices[0]).length();
        int minIx = 0;
        for(unsigned int i = 0; i < vertices.size(); i++) {
            double distance = (pos - vertices[i]).length();
            if(distance < minDistance) {
                minDistance = distance;
                minIx = i;
            }

        }
        return minIx;
    }

    bool NonManifoldMesh::isSurfaceVertex(int ix) const {
        bool isSurface = false;

        for(unsigned int i = 0; i < vertices[ix].sizeEdge(); i++) {
            Edge edge = edges[vertices[ix].edge(i)];
            isSurface = isSurface || (edge.getFaces().size() > 0);
        }
        return isSurface;
    }

    vector<TKey> NonManifoldMesh::getNeighboringVertexIndices(TKey vertexIx) {
        vector<TKey> neighbors;
        for(unsigned int i = 0; i < vertices[vertexIx].sizeEdge(); i++) {
            if(edges[vertices[vertexIx].edge(i)].vertex(0) == vertexIx) {
                neighbors.push_back(edges[vertices[vertexIx].edge(i)].vertex(1));
            } else {
                neighbors.push_back(edges[vertices[vertexIx].edge(i)].vertex(0));
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
            maxPos[0] = getSizeX()-1;
            maxPos[1] = getSizeY()-1;
            maxPos[2] = getSizeZ()-1;
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

        for(unsigned int i = 0;  i < edges.size(); i++) {
            Vertex v1 = vertices[edges[i].vertex(0)];
            Vertex v2 = vertices[edges[i].vertex(1)];
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

        inFile>>strTemp;
        //printf("[%s]\n", strTemp);
        inFile>>nVertices>>nFaces>>nEdges;
        //printf("[%d] [%d] [%d]\n", nVertices, nFaces, nEdges);

        NonManifoldMesh mesh;
        for(int i=0, lVertices=0; i < nVertices; i++, lVertices++) {
            float xPos, yPos, zPos;
            inFile>>xPos>>yPos>>zPos;
            //printf("[%f] [%f] [%f]\n", xPos, yPos, zPos);
            mesh.addVertex(Vec3F(xPos, yPos, zPos));
            inFile>>strTemp;
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
                        inFile>>faceNodes[i];
                    }

                    if((faceNodes[0] != faceNodes[1]) && (faceNodes[0] != faceNodes[2]) && (faceNodes[0] != faceNodes[3])
                            && (faceNodes[1] != faceNodes[2]) && (faceNodes[1] != faceNodes[3]) && (faceNodes[2] != faceNodes[3])) {
                        mesh.addQuad(faceNodes[0], faceNodes[1], faceNodes[2], faceNodes[3]);
                    } else {
                        mesh.addEdge(faceNodes[0], faceNodes[2]);
                    }
                    break;
                default :
                    lFaces++;
                    for(int i = 0; i < nFaceNodes; i++) {
                        //                        fscanf(inFile, "");
                        inFile>>faceNodes[i];
                    }
                    for(int i = 2; i < nFaceNodes; i++) {
                        Vec3U temp(faceNodes[0], faceNodes[i-1], faceNodes[i]);
                        mesh.addTriangle(temp);
                    }
                    break;

            }
            inFile>>strTemp;
        }

        //printf(" Vertices %d of %d loaded.  Faces %d of %d loaded", lVertices, nVertices, lFaces, nFaces);

        inFile.close();
        return mesh;
    }

}
