/*
 * NonManifoldMesh.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "NonManifoldMesh.h"

namespace Protein_Morph {

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
        addTriangle(vertexHash);
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
                glBegin(GL_LINE_STRIP);
                for(unsigned int j = 0; j < faces[i].vertexIds.size(); j++) {
                    int k = getVertexIndex(faces[i].vertexIds[j]);
                    float vals[3];
                    vals[0] = vertices[k].position.X();
                    vals[1] = vertices[k].position.Y();
                    vals[2] = vertices[k].position.Z();
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

                  map<unsigned int, TVertex> mapVertices;
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
                glBegin(GL_POLYGON);
                for(unsigned int j = 0; j < faces[i].vertexIds.size(); j++) {
                    Vec3F normal;
                    if(smoothSurfaceNormals) {
                        normal = getVertexNormal(faces[i].vertexIds[j]);
                    } else {
                        normal = getFaceNormal(i);
                    }
                    int k = getVertexIndex(faces[i].vertexIds[j]);
                    glNormal3f(normal.X(), normal.Y(), normal.Z());
                    float vals[3];
                    vals[0] = vertices[k].position.X();
                    vals[1] = vertices[k].position.Y();
                    vals[2] = vertices[k].position.Z();
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
                if(edges[i].faceIds.size() == 0) {
                    if(annotateLines) {
                        glLoadName(i);
                    }
                    glBegin(GL_LINES);
                    int k = getVertexIndex(edges[i].vertexIds[0]);
                    glVertex3f(vertices[k].position.X(), vertices[k].position.Y(), vertices[k].position.Z());
                    k = getVertexIndex(edges[i].vertexIds[1]);
                    glVertex3f(vertices[k].position.X(), vertices[k].position.Y(), vertices[k].position.Z());
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
                if(vertices[i].edgeIds.size() == 0) {
                    if(annotatePoints) {
                        glLoadName(i);
                    }
                    glBegin(GL_POINTS);
                    glVertex3f(vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
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


    void NonManifoldMesh::clear() {
        vertices.clear();
        edges.clear();
        faces.clear();

        vertexHashMap.clear();
    }


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
        TVertex tempVertex;
        tempVertex.edgeIds.clear();
        for(int x = 0; x < src.getSizeX(); x++) {
            for(int y = 0; y < src.getSizeY(); y++) {
                for(int z = 0; z < src.getSizeZ(); z++) {
                    int index = src.getIndex(x, y, z);
                    vertexLocations[index] = -1;
                    int value = (int)round(src(index));
                    if(value > 0) {
                        tempVertex.position = Vec3F(x, y, z);
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

    bool NonManifoldMesh::isEdgePresent(int vertexId1, int vertexId2) {
        bool isPresent = false;
        int v1Index = getVertexIndex(vertexId1);
        int v2Index = getVertexIndex(vertexId2);
        for(unsigned int i = 0; (i < vertices[v1Index].edgeIds.size()) && !isPresent; i++) {
            isPresent = ((int)edges[getEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[0] == v2Index) || ((int)edges[getEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[1] == v2Index);
        }

        return isPresent;
    }

    int NonManifoldMesh::addVertex(TVertex vertex) {
        vertex.id = vertices.size();
        vertex.valid = true;
        vertices.push_back(vertex);

        return vertex.id;
    }

    int NonManifoldMesh::addVertex(Vec3F location) {
        TVertex v;
        v.position = location;

        return addVertex(v);
    }

    int NonManifoldMesh::addEdge(Edge edge) {
        edge.id = edges.size();
        edge.valid = true;
        edges.push_back(edge);

        return edge.id;
    }

    int NonManifoldMesh::addFace(Face face) {
        face.id = faces.size();
        face.valid = true;
        faces.push_back(face);

        return face.id;
    }

    int NonManifoldMesh::getVertexIndex(int vertexId) {
        return vertexId;
    }

    int NonManifoldMesh::getFaceIndex(int faceId) {
        return faceId;
    }

    int NonManifoldMesh::getEdgeIndex(int edgeId) const {
        return edgeId;
    }

    int NonManifoldMesh::getEdgeIndex(int vertexId1, int vertexId2) const {
        int edgeId = -1;
        for(int i = 0; i < vertices[vertexId1].edgeIds.size(); i++) {
            if((edges[vertices[vertexId1].edgeIds[i]].vertexIds[0] == vertexId2) ||
                    (edges[vertices[vertexId1].edgeIds[i]].vertexIds[1] == vertexId2)) {
                edgeId = vertices[vertexId1].edgeIds[i];
            }
        }
        return edgeId;
    }

    void NonManifoldMesh::addEdge(int vertexId1, int vertexId2, string tag){
        Edge edge;
        edge.tag = tag;
        edge.faceIds.clear();
        edge.vertexIds[0] = vertexId1;
        edge.vertexIds[1] = vertexId2;
        int edgeId = addEdge(edge);
        vertices[getVertexIndex(vertexId1)].edgeIds.push_back(edgeId);
        vertices[getVertexIndex(vertexId2)].edgeIds.push_back(edgeId);
    }

    void NonManifoldMesh::addQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, string newEdgeTag, string faceTag) {
        Vec3U v123(vertexId1, vertexId2, vertexId3);
        addTriangle(v123, newEdgeTag, faceTag);

        Vec3U v134(vertexId1, vertexId3, vertexId4);
        addTriangle(v134, newEdgeTag, faceTag);
    }

    void NonManifoldMesh::addTriangle(Vec3U vertexId, string newEdgeTag, string faceTag) {
        int vertexId1 = vertexId[0];
        int vertexId2 = vertexId[1];
        int vertexId3 = vertexId[2];

        if(!isEdgePresent(vertexId1, vertexId2))
            addEdge(vertexId1, vertexId2, newEdgeTag);

        if(!isEdgePresent(vertexId2, vertexId3))
            addEdge(vertexId2, vertexId3, newEdgeTag);

        if(!isEdgePresent(vertexId3, vertexId1))
            addEdge(vertexId3, vertexId1, newEdgeTag);

        Face face;
        face.tag = faceTag;
        face.vertexIds.clear();
        face.vertexIds.push_back(vertexId1);
        face.vertexIds.push_back(vertexId2);
        face.vertexIds.push_back(vertexId3);
        face.edgeIds.clear();

        int vertexIds[4] = {vertexId1, vertexId2, vertexId3, vertexId1};

        for(int i = 0; i < 3; i++) {
            int vertexIndex = getVertexIndex(vertexIds[i]);
            for (int j = 0; j < (int)vertices[vertexIndex].edgeIds.size(); j++) {
                int edgeIndex = getEdgeIndex(vertices[vertexIndex].edgeIds[j]);

                if( (((int)edges[edgeIndex].vertexIds[0] == vertexIds[i])   && ((int)edges[edgeIndex].vertexIds[1] == vertexIds[i+1])) ||
                        (((int)edges[edgeIndex].vertexIds[0] == vertexIds[i+1]) && ((int)edges[edgeIndex].vertexIds[1] == vertexIds[i]))) {
                    face.edgeIds.push_back(vertices[vertexIndex].edgeIds[j]);
                }
            }
        }

        int faceId = addFace(face);
        for(int i = 0; i < (int)face.edgeIds.size(); i++)
            edges[getEdgeIndex(face.edgeIds[i])].faceIds.push_back(faceId);
    }

    void NonManifoldMesh::mergeMesh(const NonManifoldMesh & src) {
        vector<int> indices;

        for(unsigned int i = 0; i < src.vertices.size(); i++)
            indices.push_back(addVertex(src.vertices[i]));

        for(unsigned int i = 0; i < src.edges.size(); i++)
            addEdge(indices[src.edges[i].vertexIds[0]], indices[src.edges[i].vertexIds[1]], src.edges[i].tag);

        for(unsigned int i = 0; i < src.faces.size(); i++) {
            if(src.faces[i].vertexIds.size() == 3) {
                Vec3U temp(indices[src.faces[i].vertexIds[0]],
                                      indices[src.faces[i].vertexIds[1]],
                                      indices[src.faces[i].vertexIds[2]]);
                addTriangle(temp, NULL, src.faces[i].tag);
            }
            else if(src.faces[i].vertexIds.size() == 3)
                addQuad(indices[src.faces[i].vertexIds[0]],
                        indices[src.faces[i].vertexIds[1]],
                        indices[src.faces[i].vertexIds[2]],
                        indices[src.faces[i].vertexIds[3]], NULL,
                        src.faces[i].tag);
        }
    }

    void NonManifoldMesh::removeFace(int faceId) {
        int faceIndex = getFaceIndex(faceId);
        faces[faceIndex].valid = false;

        for(unsigned int i = 0; i < faces[faceIndex].edgeIds.size(); i++) {
            int edgeIndex = getEdgeIndex(faces[faceIndex].edgeIds[i]);
            for(int j = (int)edges[edgeIndex].faceIds.size()-1; j >= 0; j--) {
                if(edges[edgeIndex].faceIds[j] == faceId) {
                    edges[edgeIndex].faceIds.erase(edges[edgeIndex].faceIds.begin() + j);
                }
            }
        }
    }

    void NonManifoldMesh::removeEdge(int edgeId) {
        int edgeIndex = getEdgeIndex(edgeId);

        if(edges[edgeIndex].faceIds.size() > 0) {
            printf("Cannot remove edgeId %i as it has face associations\n", edgeId);
            exit(0);
        }
        edges[edgeIndex].valid = false;

        for(int i = 0; i < 2; i++) {
            int vertexIndex = getVertexIndex(edges[edgeIndex].vertexIds[i]);
            for(int j = (int)vertices[vertexIndex].edgeIds.size()-1; j >= 0; j--) {
                if((int)vertices[vertexIndex].edgeIds[j] == edgeId) {
                    vertices[vertexIndex].edgeIds.erase(vertices[vertexIndex].edgeIds.begin() + j);
                }
            }
        }
    }

    void NonManifoldMesh::removeVertex(int vertexId) {
        int vertexIndex = getVertexIndex(vertexId);
        vertices[vertexIndex].valid = false;

        if(vertices[vertexIndex].edgeIds.size() > 0) {
            printf("Cannot remove vertexId %i as it has edge associations\n", vertexId);
            exit(0);
        }
    }

    void NonManifoldMesh::removeNullEntries() {
        for(int i = (int)vertices.size()-1; i >= 0; i--) {
            if(!vertices[i].valid)
                vertices.erase(vertices.begin() + i);
        }


        for(unsigned int i = 0; i < vertices.size(); i++) {
            if(vertices[i].id != i) {
                int oldId = vertices[i].id;
                int newId = i;
                vertices[i].id = newId;
                for(unsigned int j = 0; j < vertices[i].edgeIds.size(); j++) {
                    for(int k = 0; k < 2; k++) {
                        if((int)edges[getEdgeIndex(vertices[i].edgeIds[j])].vertexIds[k] == oldId) {
                            edges[getEdgeIndex(vertices[i].edgeIds[j])].vertexIds[k] = newId;
                        }
                    }
                }
            }
        }

        for(int i = (int)edges.size()-1; i >= 0; i--) {
            if(!edges[i].valid) {
                edges.erase(edges.begin() + i);
            }
        }

        for(unsigned int i = 0; i < edges.size(); i++) {
            if(edges[i].id != i) {
                int oldId = edges[i].id;
                int newId = i;
                edges[i].id = newId;

                for(int j = 0; j < 2; j++)  {
                    for(unsigned int k = 0; k < vertices[getVertexIndex(edges[i].vertexIds[j])].edgeIds.size(); k++) {
                        if((int)vertices[getVertexIndex(edges[i].vertexIds[j])].edgeIds[k] == oldId) {
                            vertices[getVertexIndex(edges[i].vertexIds[j])].edgeIds[k] = newId;
                        }
                    }
                }

                for(unsigned int j = 0; j < edges[i].faceIds.size(); j++)  {
                    for(unsigned int k = 0; k < faces[getFaceIndex(edges[i].faceIds[j])].edgeIds.size(); k++) {
                        if((int)faces[getFaceIndex(edges[i].faceIds[j])].edgeIds[k] == oldId) {
                            faces[getFaceIndex(edges[i].faceIds[j])].edgeIds[k] = newId;
                        }
                    }
                }
            }
        }

        for(int i = (int)faces.size()-1; i >= 0; i--) {
            if(!faces[i].valid) {
                faces.erase(faces.begin() + i);
            }
        }

        for(unsigned int i = 0; i < faces.size(); i++) {
            if(faces[i].id != i) {
                int oldId = faces[i].id;
                int newId = i;
                faces[i].id = newId;
                for(unsigned int j = 0; j < faces[i].edgeIds.size(); j++) {
                    for(unsigned int k = 0; k < edges[getEdgeIndex(faces[i].edgeIds[j])].faceIds.size(); k++) {
                        if((int)edges[getEdgeIndex(faces[i].edgeIds[j])].faceIds[k] == oldId) {
                            edges[getEdgeIndex(faces[i].edgeIds[j])].faceIds[k] = newId;
                        }
                    }
                }
            }
        }
    }

    void NonManifoldMesh::toOffCells(string fileName) {
        removeNullEntries();
        ofstream outFile(fileName.c_str());
        outFile<<"OFF\n";
        outFile<<vertices.size()
               <<" "<<faces.size() + edges.size()
               <<" 0"
               <<endl;

        for(int i = 0; i < (int)vertices.size(); i++) {
            outFile<<" "<<getOriginX() + getSpacingX() * vertices[i].position.X()
                   <<" "<<getOriginY() + getSpacingY() * vertices[i].position.Y()
                   <<" "<<getOriginZ() + getSpacingZ() * vertices[i].position.Z()
                   <<endl;
        }
        int lastVertex;
        for(int i = 0; i < (int)faces.size(); i++) {
            outFile<<faces[i].edgeIds.size();
            lastVertex = -1;

            for(int j =0; j < (int)faces[i].vertexIds.size(); j++) {
                outFile<<getVertexIndex(faces[i].vertexIds[j]);
            }

            outFile<<endl;
        }

        for(int i = 0; i < (int)edges.size(); i++) {
            outFile<<"4"
                   <<" "<<edges[i].vertexIds[0]
                   <<" "<<edges[i].vertexIds[0]
                   <<" "<<edges[i].vertexIds[1]
                   <<" "<<edges[i].vertexIds[1]
                   <<endl;
        }
        outFile.close();
    }

    void NonManifoldMesh::toMathematicaFile(string fileName) {
        removeNullEntries();
        ofstream outF(fileName.c_str());
        // Vertices
        outF<<"{\n";
        outF<<"{";
        for(unsigned int i = 0; i < vertices.size(); i++) {
            outF<<"{"
                    <<vertices[i].position.X()
                    <<" "<<vertices[i].position.Y()
                    <<" "<<vertices[i].position.Z()
                    <<"}";
            if(i != (int)vertices.size()-1) {
                outF<<", ";
            }
        }
        outF<<"},\n";

        // Edges
        outF<<"{";
        for(unsigned int i = 0; i < edges.size(); i++) {
            outF<<"{"
                    <<edges[i].vertexIds[0]+1
                    <<" "<<edges[i].vertexIds[1]+1
                    <<"}";
            if(i != (int)edges.size()-1) {
                outF<<", ";
            }
        }
        outF<<"},\n";

        // Faces
        outF<<"{";

        for(unsigned int i = 0; i < faces.size(); i++) {
            int lastVertex = -1;
            outF<<"{";
            for(int j = (int)faces[i].edgeIds.size()-1; j >= 0; j--) {
                if((edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[0]) ||
                        (edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[1])) {
                    lastVertex = edges[faces[i].edgeIds[j]].vertexIds[1];
                } else {
                    lastVertex = edges[faces[i].edgeIds[j]].vertexIds[0];
                }
                outF<<lastVertex+1;
                if(j != 0) {
                    outF<<", ";
                }
            }
            outF<<"}";

            if(i != (int)faces.size()-1) {
                outF<<", ";
            }
        }
        outF<<"}\n";
        outF<<"}";
        outF.close();
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
                minPos[j] = min(minPos[j], (double)vertices[i].position[j]);
                maxPos[j] = max(maxPos[j], (double)vertices[i].position[j]);
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
            TVertex v1 = vertices[getVertexIndex(edges[i].vertexIds[0])];
            TVertex v2 = vertices[getVertexIndex(edges[i].vertexIds[1])];
            vector<Vec3I> positions = Rasterizer::ScanConvertLineC8(v1.position.XInt(), v1.position.YInt(), v1.position.ZInt(), v2.position.XInt(), v2.position.YInt(), v2.position.ZInt());
            for(unsigned int j = 0; j < positions.size(); j++) {
                vol(positions[j] - minPosInt) = 1.0;
            }
        }

        vol.setOrigin(origin);
        vol.setSpacing(spacing);
        return vol;
    }

    Vec3F NonManifoldMesh::getVertexNormal(int vertexId) {
        int index = getVertexIndex(vertexId);
        Vec3F normal(0,0,0);
        for(unsigned int i = 0; i < vertices[index].edgeIds.size(); i++) {
            int edgeIndex = getEdgeIndex(vertices[index].edgeIds[i]);
            for(unsigned int j = 0; j < edges[edgeIndex].faceIds.size(); j++) {
                normal += getFaceNormal(edges[edgeIndex].faceIds[j]);
            }
        }
        normal.normalize();
        return normal;
    }

    Vec3F NonManifoldMesh::getFaceNormal(int faceId) {
        Face face = faces[getFaceIndex(faceId)];
        Vec3F normal(1,0,0);

        if(face.vertexIds.size() >= 3) {
            normal = (vertices[getVertexIndex(face.vertexIds[1])].position - vertices[getVertexIndex(face.vertexIds[0])].position) ^
                    (vertices[getVertexIndex(face.vertexIds[2])].position - vertices[getVertexIndex(face.vertexIds[0])].position);
            normal.normalize();
        }
        return normal;
    }

    NonManifoldMesh NonManifoldMesh::smoothLaplacian(double converganceRate) {
        NonManifoldMesh smoothedMesh(*this);
        for(int i = 0; i < (int)vertices.size(); i++) {
            TVertex vertex = vertices[i];
            if(vertex.valid) {
                Vec3F newPosition;
                if(vertex.edgeIds.size() > 0) {
                    newPosition = Vec3F(0,0,0);
                    for(int j = 0; j < (int)vertex.edgeIds.size(); j++) {
                        int vertexIndex;
                        if((int)edges[getEdgeIndex(vertex.edgeIds[j])].vertexIds[0] == i) {
                            vertexIndex = 1;
                        } else {
                            vertexIndex = 0;
                        }

                        newPosition = newPosition + vertices[getVertexIndex(edges[getEdgeIndex(vertex.edgeIds[j])].vertexIds[vertexIndex])].position;
                    }
                    newPosition = newPosition * (1.0/vertex.edgeIds.size());
                } else {
                    newPosition = vertex.position;
                }
                smoothedMesh.vertices[i].position = smoothedMesh.vertices[i].position * (1.0 - converganceRate)+ newPosition * converganceRate;
            }
        }
        return smoothedMesh;
    }

    NonManifoldMesh NonManifoldMesh::smoothLaplacian(double converganceRate, int iterations) {
        NonManifoldMesh oldMesh(*this);

        for(int i = 0; i < iterations; i++) {
            oldMesh = oldMesh.smoothLaplacian(converganceRate);
        }

        return oldMesh;
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

    vector<unsigned int> NonManifoldMesh::getPath(unsigned int edge0Ix, unsigned int edge1Ix) {
        vector<unsigned int> path;
        map<unsigned int,  unsigned int> source;

        queue<unsigned int> edgeList;
        edgeList.push(edge0Ix);

        bool found = false;

        while((edgeList.size() > 0) && !found) {
            unsigned int currentEdge = edgeList.front();
            edgeList.pop();
            found = currentEdge == edge1Ix;
            if(!found) {
                for(unsigned int v = 0; v < 2; v++) {
                    unsigned int vertexIx = getVertexIndex(edges[currentEdge].vertexIds[v]);
                    for(unsigned int e = 0; e < vertices[vertexIx].edgeIds.size(); e++) {
                        unsigned int edgeIx = getEdgeIndex(vertices[vertexIx].edgeIds[e]);
                        if(source.find(edgeIx) == source.end()) {
                            source[edgeIx] = currentEdge;
                            edgeList.push(edgeIx);
                        }
                    }
                }
            }
        }

        if(found) {
            unsigned int currentEdge = edge1Ix;
            path.push_back(currentEdge);
            while(currentEdge != edge0Ix) {
                currentEdge = source[currentEdge];
                path.push_back(currentEdge);
            }
        }

        return path;
    }

    vector<Vec3F> NonManifoldMesh::sampleTriangle(int faceId, double discretizationStep) {
        int faceIndex = getFaceIndex(faceId);
        Face face = faces[faceIndex];

        vector<Vec3F> points;
        if(face.vertexIds.size() != 3) {
            printf("ERROR: Sampling a polygon NOT a triangle!\n");
            return points;
        } else {
            TVertex p = vertices[getVertexIndex(face.vertexIds[0])];
            TVertex q = vertices[getVertexIndex(face.vertexIds[1])];
            TVertex r = vertices[getVertexIndex(face.vertexIds[2])];
            Vec3F v1 = q.position - p.position;
            Vec3F v2 = r.position - p.position;
            double v1Length = v1.length();
            double v2Length = v2.length();
            v1.normalize();
            v2.normalize();

            for(double a1 = 0; a1 <= v1Length; a1 += discretizationStep) {
                for(double a2 = 0; a2 <= v2Length; a2 += discretizationStep) {
                    if(a1/v1Length + a2/v2Length <= 1) {
                        points.push_back(p.position + v1 * a1 + v2 * a2);
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

        double minDistance = (pos - vertices[0].position).length();
        int minIx = 0;
        for(unsigned int i = 0; i < vertices.size(); i++) {
            double distance = (pos - vertices[i].position).length();
            if(distance < minDistance) {
                minDistance = distance;
                minIx = i;
            }

        }
        return minIx;
    }

    bool NonManifoldMesh::isSurfaceVertex(int ix) const {
        bool isSurface = false;

        for(unsigned int i = 0; i < vertices[ix].edgeIds.size(); i++) {
            Edge edge = edges[getEdgeIndex(vertices[ix].edgeIds[i])];
            isSurface = isSurface || (edge.faceIds.size() > 0);
        }
        return isSurface;
    }

    vector<unsigned int> NonManifoldMesh::getNeighboringVertexIndices(unsigned int vertexIx) {
        vector<unsigned int> neighbors;
        for(unsigned int i = 0; i < vertices[vertexIx].edgeIds.size(); i++) {
            if(edges[vertices[vertexIx].edgeIds[i]].vertexIds[0] == vertexIx) {
                neighbors.push_back(edges[vertices[vertexIx].edgeIds[i]].vertexIds[1]);
            } else {
                neighbors.push_back(edges[vertices[vertexIx].edgeIds[i]].vertexIds[0]);
            }
        }
        return neighbors;
    }

}
