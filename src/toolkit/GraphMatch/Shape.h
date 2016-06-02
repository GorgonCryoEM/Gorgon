#ifndef TOOLKIT_GRAPHMATCH_SHAPE_H
#define TOOLKIT_GRAPHMATCH_SHAPE_H

#include "MathTools/VectorMath.h"
//#include <vector>
//#include <MathTools/MathLib.h>
#include "Core/GlobalConstants.h"
#include "GraphMatch/Constants.h"

//using namespace std;
using namespace MathTools;

namespace GraphMatch {

    struct Polygon  {
        int pointIndex1;
        int pointIndex2;
        int pointIndex3;
        int pointIndex4;
    };

    class Shape {
    public:
        Shape();
        bool getSelected();
        bool isHelix();
        bool isSheet();
        bool isInsideShape(Vec3D p);
        bool isInsideShape(Vec3F p);
        double minimumDistanceToPoint(Vec3D P);
        double getHeight();
        double getRadius();
        double getCornerCellsMaxLength();
        vector<Vec3D> getMaxLengthCorners();
        int getLocationInVector(vector<Point3Pair> v, Point3Pair point);
        int getType();
        Matrix4 getRotationMatrix();
        Matrix4 getWorldToObjectMatrix();
        Vec3D getCenter();
        void addInternalCell(Point3Pair point);
        void findCornerCellsInHelix();
        void rotate(Vector3<double> axis, double angle);
        void translate(Vector3<double> translationVector);
        void setCenter(Vec3D center);
        void setCenter(Vec3F center);
        void setColor(float r, float g, float b, float a);
        void setHeight(double height);
        void setRadius(double radius);
        void getRotationAxisAndAngle(Vec3F &axis, double &angle);
        static Shape * createHelix(Vec3F p1, Vec3F p2, float radius);
        static void writeToFile(vector<Shape*> & helices, FILE * fileName);
        void getColor(float & r, float & g, float & b, float & a);
        void setSelected(bool selected);

        virtual void setObjectSpecificColoring(bool objectSpecific);

        Vec3D getWorldCoordinates(Vec3D point);
        Point3Pair getCornerCell(int node);
        Vec3F getCornerCell2(int node);
        Vec3F getCornerCell3(int node);

    private:
        bool isInsideCylinder(Vec3D point);
        bool isInsidePolygon(Vec3D point);
        void scale(double x, double y, double z);
        void updateWorldToObjectMatrix();

    public:
        int shapeType;
        float length;
        vector<Point3Pair> internalCells;
        vector<Point3Pair> cornerCells;
        vector<Vec3D> polygonPoints;
        vector<Polygon> polygons;
        Vec3F internalToRealScale;
        Vec3F internalToRealOrigin;
        bool isObjectSpecificColoring;

    private:
        Matrix4 worldToObject;
        Matrix4 objectToWorld;
        Vec3D	centerPoint;
        double  radius;
        double  height;
        Matrix4 rotationMatrix;
        Matrix4 inverseRotationMatrix;
        float colorR;
        float colorG;
        float colorB;
        float colorA;
        bool selected;
    };

    inline Shape::Shape() {
        worldToObject = Matrix4::identity();
        objectToWorld = Matrix4::identity();
        rotationMatrix = Matrix4::identity();
        inverseRotationMatrix = Matrix4::identity();
        internalCells.clear();
        colorR = 0.0f;
        colorG = 1.0f;
        colorB = 0.0f;
        colorA = 1.0f;
        selected = false;

        isObjectSpecificColoring = false;
    }

    inline bool Shape::getSelected() {
        return selected;
    }

    inline bool Shape::isHelix() {
        return (shapeType == GRAPHEDGE_HELIX);
    }

    inline bool Shape::isSheet() {
        return (shapeType != GRAPHEDGE_HELIX);
    }

    inline bool Shape::isInsideShape(Vec3D point) {
        Vec3D newPoint(point[0], point[1], point[2]);
        if(shapeType == GRAPHEDGE_HELIX) {
            return isInsideCylinder(newPoint);
        } else {
            return isInsidePolygon(newPoint);
        }
    }

    inline void Shape::setObjectSpecificColoring(bool objectSpecific) {
        isObjectSpecificColoring = objectSpecific;
    }

    inline bool Shape::isInsideShape(Vec3F p) {
        return isInsideShape(Vec3D(p.X(), p.Y(), p.Z()));
    }

    inline bool Shape::isInsidePolygon(Vec3D p) {
        Polygon poly;
        Vec3D a,b,c,q;
        double l1, l2;
        Vector3<double> n;
        double d;
        bool isInside = false;
        double A,B,C,D,E,F,G,H,I;
        for(unsigned int i = 0; i < polygons.size(); i++) {
            poly = (Polygon)polygons[i];
            // read triangle vertices
            a = (Vec3D)polygonPoints[poly.pointIndex1];
            b = (Vec3D)polygonPoints[poly.pointIndex2];
            c = (Vec3D)polygonPoints[poly.pointIndex3];
            // find surface normal
            n = ((b-a)^(c-a)) / ((b-a)^(c-a)).length();
            // measure distance from point p to triangle
            d = n * (p - a);
            // find projection of p onto triangle
            q = p + n*d;

            A = a[0]-c[0];
            B = b[0]-c[0];
            C = c[0]-q[0];
            D = a[1]-c[1];
            E = b[1]-c[1];
            F = c[1]-q[1];
            G = a[2]-c[2];
            H = b[2]-c[2];
            I = c[2]-q[2];

            l1 = (B*(F + I) - C*(E + H)) / (A*(E + H) - B*(D + G));
            l2 = (A*(F + I) - C*(D + G)) / (B*(D + G) - A*(E + H));

            isInside = isInside || ((abs(d) < 1) && (l1 >= 0) && (l1 <= 1) && (l2 >= 0) && (l2 <= 1));
        }
        return isInside;
    }

    // returns the minimum distance between a point p and a Shape
    inline double Shape::minimumDistanceToPoint(Vec3D P) {
        Vec3D pt;
        double d;
        double dmin = MAXDOUBLE;

        // find min distance from point p to any triangle in the polygon
        for(unsigned int i = 0; i < polygonPoints.size(); i++) {
            pt = (Vec3D)polygonPoints[i];
            d = (pt - P).length();
            dmin = min(d, dmin);
        }
        return dmin;
    }

    inline bool Shape::isInsideCylinder(Vec3D point) {
        point = objectToWorld * point;
        return ((point[0]*point[0] + point[2]*point[2] <= 0.25) && (abs(point[1]) <= 0.5));
    }

    inline double Shape::getHeight() {
        return height;
    }

    inline double Shape::getRadius(){
        return radius;
    }

    inline int Shape::getLocationInVector(vector<Point3Pair> v, Point3Pair point) {
        int loc = -1;
        for(unsigned int i = 0; (i < v.size() && loc < 0); i++) {
            if(v[i] == point) {
                loc = i;
            }
        }
        return loc;
    }

    inline int Shape::getType() {
        return shapeType;
    }

    inline Matrix4 Shape::getRotationMatrix() {
        return rotationMatrix;
    }

    inline Matrix4 Shape::getWorldToObjectMatrix() {
        return worldToObject;
    }
    inline Vec3D Shape::getCenter() {
        return centerPoint;
    }

    inline Vec3D Shape::getWorldCoordinates(Vec3D point) {
        return worldToObject * point;
    }

    inline Point3Pair Shape::getCornerCell(int node) {
        for(unsigned int i = 0; i < cornerCells.size(); i++) {
            if(cornerCells[i].node == node) {
                return cornerCells[i];
            }
        }
        printf("Error <Shape, GetCornerCell>: Corner cell %d not found\n", node);
        return Point3Pair(0,0,0,0);
    }

    inline Vec3F Shape::getCornerCell2(int node) {
        Point3Pair cell = getCornerCell(node);
        return Vec3F((float)cell.x, (float)cell.y, (float)cell.z);
    }

    inline Vec3F Shape::getCornerCell3(int node) {
        Vec3D pt;

        if(node == 1) {
            pt = getWorldCoordinates(Vec3D(0, -0.5, 0));
        } else {
            pt = getWorldCoordinates(Vec3D(0, 0.5, 0));
        }
        return Vec3F((float)pt[0], (float)pt[1], (float)pt[2]);
    }

    inline void Shape::addInternalCell(Point3Pair point) {
        internalCells.push_back(point);
    }

    // Search through all voxels inside a helix to find the two corners, which have only one neighbor in the helix.
    inline void Shape::findCornerCellsInHelix() {

        // array to help iterate over 6 neighbor voxels
        int d[6][3];
        d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
        d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
        d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
        d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
        d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
        d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;

        // counter of number of neighbor cells are inside the helix
        int insideCounter;

        // for each cell inside the helix
        for(unsigned int i = 0; i < internalCells.size(); i++) {
            insideCounter = 0;
            // count the number of neighbor cells inside the helix
            for(int j = 0; j < 6; j++) {
                if(getLocationInVector(internalCells, Point3Pair(internalCells[i].x + d[j][0], internalCells[i].y + d[j][1], internalCells[i].z + d[j][2], 0)) >= 0) {
                    insideCounter++;
                }
            }
            // if only one neighbor inside the helix, this is a corner cell. add it to the list.
            if(insideCounter == 1) {
                cornerCells.push_back(internalCells[i]);
            }
        }

        // abort if more than two corner cells were found
        assert(cornerCells.size() >= 2);

        double maxDistance = -1;
        double dist1, dist2;
        int corner1 = 0;
        int corner2 = cornerCells.size() - 1;

        for(int i = 0; i < (int)cornerCells.size() - 1; i++){
            for(unsigned int j = i+1; j < cornerCells.size(); j++) {
                dist1 = Point3Pair::EuclideanDistance(cornerCells[i], cornerCells[j]);
                if(maxDistance < dist1) {
                    corner1 = i;
                    corner2 = j;
                    maxDistance = dist1;
                }
            }
        }

        Vec3F actualCorner1 = getCornerCell3(1);
        Vec3F actualCorner2 = getCornerCell3(2);

        Vec3F c1, c2;

        c1 = Vec3F(
            internalToRealOrigin.X() + (float)cornerCells[corner1].x * internalToRealScale.X(),
            internalToRealOrigin.Y() + (float)cornerCells[corner1].y * internalToRealScale.Y(),
            internalToRealOrigin.Z() + (float)cornerCells[corner1].z * internalToRealScale.Z());
        c2 = Vec3F(
            internalToRealOrigin.X() + (float)cornerCells[corner2].x * internalToRealScale.X(),
            internalToRealOrigin.Y() + (float)cornerCells[corner2].y * internalToRealScale.Y(),
            internalToRealOrigin.Z() + (float)cornerCells[corner2].z * internalToRealScale.Z());

        if((actualCorner1-c1).length() > (actualCorner1-c2).length()) {
            int temp = corner1;
            corner1 = corner2;
            corner2 = temp;
        }

        cornerCells[corner1].node = 1;
        cornerCells[corner2].node = 2;

        for(unsigned int i = 0; i < cornerCells.size(); i++) {
            dist1 = Point3Pair::EuclideanDistance(cornerCells[corner1], cornerCells[i]);
            dist2 = Point3Pair::EuclideanDistance(cornerCells[corner2], cornerCells[i]);
            if((dist1 > BORDER_MARGIN_THRESHOLD) && (dist2 > BORDER_MARGIN_THRESHOLD)) {
                cornerCells[i].node = 0;
            } else if(dist1 < dist2) {
                cornerCells[i].node = 1;
            } else {
                cornerCells[i].node = 2;
            }
        }

        for(int i = (int)cornerCells.size() - 1; i >= 0; i--) {
            if(cornerCells[i].node == 0) {
                cornerCells.erase(cornerCells.begin() + i);
            }
        }
        if(cornerCells.size() < 2) {
            printf("Error <Shape, FindCornerCellsInHelix>: 2 corner cells not found\n");
        }

        assert(cornerCells.size() >= 2);
    }

    inline double Shape::getCornerCellsMaxLength() {
        double length = 0;
        for(int i = 0; i < (int)cornerCells.size() - 1; i++) {
            for(int j = i+1; j < (int)cornerCells.size(); j++) {
                length = max(length, Point3Pair::EuclideanDistance(cornerCells[i], cornerCells[j]));
            }
        }
        return length;
    }

    inline vector<Vec3D> Shape::getMaxLengthCorners() {
        cout << "getting max length corners " <<  (int)polygonPoints.size() << endl;
        double length = 0.0;
        vector<Vec3D> result;
        for(int i = 0; i < (int)polygonPoints.size() - 1; i++) {
            cout << "in first loop" << endl;
            for(int j = i+1; j < (int)polygonPoints.size(); j++) {
                cout << "in second loop" << endl;
                if((polygonPoints[i] - polygonPoints[j]).length() > length){
                    length = (polygonPoints[i] - polygonPoints[j]).length();
                    result.clear();
                    result.push_back(polygonPoints[i]);
                    result.push_back(polygonPoints[i]);
                    cout << "puttin in corners" << endl;
                }
            }
        }
        return result;
    }

    inline void Shape::rotate(Vector3<double> axis, double angle){
        rotationMatrix = Matrix4::rotation(axis, angle) * rotationMatrix;
        inverseRotationMatrix = inverseRotationMatrix * Matrix4::rotation(axis, -angle);
        updateWorldToObjectMatrix();
    }

    inline void Shape::translate(Vector3<double> translationVector){
        centerPoint = centerPoint + translationVector;
        updateWorldToObjectMatrix();
    }

    inline void Shape::setColor(float r, float g, float b, float a) {
        colorR = r;
        colorG = g;
        colorB = b;
        colorA = a;
    }

    inline void Shape::setCenter(Vec3D center) {
        this->centerPoint = center;
        updateWorldToObjectMatrix();
    }

    inline void Shape::setCenter(Vec3F center) {
        setCenter(Vec3D(center.X(), center.Y(), center.Z()));
    }

    inline void Shape::setHeight(double height) {
        this->height = height;
        this->length = height;
        updateWorldToObjectMatrix();
    }

    inline void Shape::setRadius(double radius) {
        this->radius = radius;
        updateWorldToObjectMatrix();
    }

    inline void Shape::getColor(float & r, float & g, float & b, float & a) {
        r = colorR;
        g = colorG;
        b = colorB;
        a = colorA;
    }

    inline void Shape::updateWorldToObjectMatrix() {
        worldToObject = Matrix4::translation(centerPoint) * rotationMatrix * Matrix4::scaling(radius*2, height, radius*2);
        objectToWorld = Matrix4::scaling(1.0/(radius*2.0), 1.0/height, 1.0/(radius*2.0)) * inverseRotationMatrix * Matrix4::translation(Vec3D(-centerPoint[0], -centerPoint[1], -centerPoint[2]));
    }

    inline void Shape::setSelected(bool selected) {
        this->selected = selected;
    }

    inline void Shape::getRotationAxisAndAngle(Vec3F &axis, double &angle) {
        double x,y,z;
        double epsilon = 0.01;
        double epsilon2 = 0.1;

        Matrix4 m = rotationMatrix;
        if ((abs(m(0, 1)-m(1,0))< epsilon) && (abs(m(0,2)-m(2,0))< epsilon) && (abs(m(1,2)-m(2,1))< epsilon)) {
            // singularity found
            // first check for identity matrix which must have +1 for all terms
            //  in leading diagonaland zero in other terms
            if ((abs(m(0,1)+m(1,0)) < epsilon2) && (abs(m(0,2)+m(2,0)) < epsilon2)
                && (abs(m(1,2)+m(2,1)) < epsilon2) && (abs(m(0,0)+m(1,1)+m(2,2)-3) < epsilon2)) {
                // this singularity is identity matrix so angle = 0
                axis = Vec3F(1,0,0);
                angle = 0;
                return;
            }
            // otherwise this singularity is angle = 180
            angle = PI;
            double xx = (m(0,0)+1.0)/2.0;
            double yy = (m(1,1)+1.0)/2.0;
            double zz = (m(2,2)+1.0)/2.0;
            double xy = (m(0,1)+m(1,0))/4.0;
            double xz = (m(0,2)+m(2,0))/4.0;
            double yz = (m(1,2)+m(2,1))/4.0;
            if ((xx > yy) && (xx > zz)) { // m[0][0] is the largest diagonal term
                if (xx< epsilon) {
                    x = 0;
                    y = 0.7071;
                    z = 0.7071;
                } else {
                    x = sqrt(xx);
                    y = xy/x;
                    z = xz/x;
                }
            } else if (yy > zz) { // m[1][1] is the largest diagonal term
                if (yy< epsilon) {
                    x = 0.7071;
                    y = 0;
                    z = 0.7071;
                } else {
                    y = sqrt(yy);
                    x = xy/y;
                    z = yz/y;
                }
            } else { // m[2][2] is the largest diagonal term so base result on this
                if (zz< epsilon) {
                    x = 0.7071;
                    y = 0.7071;
                    z = 0;
                } else {
                    z = sqrt(zz);
                    x = xz/z;
                    y = yz/z;
                }
            }
            axis = Vec3F((float)x, (float)y, (float)z);
            return;
        }
        // as we have reached here there are no singularities so we can handle normally
        double s = sqrt((m(2,1) - m(1,2))*(m(2,1) - m(1,2)) +(m(0,2) - m(2,0))*(m(0,2) - m(2,0)) + (m(1,0) - m(0,1))*(m(1,0) - m(0,1))); // used to normalise
        if (abs(s) < 0.001) {
            s = 1;
            // prevent divide by zero, should not happen if matrix is orthogonal and should be
            // caught by singularity test above, but I've left it in just in case
        }
        angle = acos(( m(0,0) + m(1,1) + m(2,2) - 1.0)/2.0);
        x = (m(2,1) - m(1,2))/s;
        y = (m(0,2) - m(2,0))/s;
        z = (m(1,0) - m(0,1))/s;
        axis = Vec3F((float)x, (float)y, (float)z);
        return;
    }

    inline Shape * Shape::createHelix(Vec3F p1, Vec3F p2, float radius) {
        Shape * newHelix = new Shape();
        newHelix->shapeType = GRAPHEDGE_HELIX;
        Vec3F center = (p1+p2) * 0.5;
        Vec3F dir = p1-p2;
        Vec3F yaxis = Vec3F(0, 1, 0);

        newHelix->setCenter(Vec3D(center.X(), center.Y(), center.Z()));
        newHelix->setRadius(radius);
        newHelix->setHeight(dir.length());
        Vec3F axis = dir^yaxis;

        dir.normalize();
        double angle = acos(dir * yaxis);
        newHelix->rotate(Vector3<double>(axis.X(), axis.Y(), axis.Z()), -angle);
        return newHelix;
    }

    inline void Shape::writeToFile(vector<Shape*> & helices, FILE * fout) {
        Vec3D center;
        Vec3F start, end, axis;
        double angle;
        float helixLength;
        fprintf(fout, "#VRML V2.0 utf8\n");

        for(unsigned int i = 0; i < helices.size(); i++) {
            center = helices[i]->getCenter();
            start = helices[i]->getCornerCell3(1);
            end = helices[i]->getCornerCell3(2);
            helixLength = (start-end).length();
            helices[i]->getRotationAxisAndAngle(axis, angle);

            fprintf(fout, "Group {\n children [\n Transform {\n  translation %f %f %f\n", center[0], center[1], center[2]);
            fprintf(fout, "  rotation %f %f %f %f\n", axis.X(), axis.Y(), axis.Z(), angle);
            fprintf(fout, "  children [\n   Shape {\n    appearance \n     Appearance {\n      material Material {\n       emissiveColor 0 0.5 0\n       }\n     }\n");
            fprintf(fout, "    geometry\n     Cylinder {\n      height %f \n      radius 2.5 \n     }\n   }\n  ]\n }\n ]\n}", helixLength);
        }
    }
}
#endif
