#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct point{
    float x, y;
} Point;
typedef struct arc{
    Point center;
    float radius;
    float startAlpha;
    float deltaAlpha;
    float thickness;
} Arc;
typedef struct circle{
    Point center;
    float radius;
} Circle;

float getDistance(Point p1, Point p2);
float getAngleBetweenPoints(Point p1, Point p2);
float toDegrees(float rad);
float toRadians(float deg);
float getRadiusFromVolume(float volume);
float getVolumeFromRadius(float radius);
int isAngleBetween(float angle, float a, float b);
int checkCircleArcCollision(Arc a, Circle c);
int checkCircleCicleCollision(Circle c1, Circle c2);

float vecLength(Point p);
float vecAngle(Point p);
Point vecFromAngleLength(float angle, float length);
Point vecFromPoints(Point p1, Point p2);
float dotProduct(Point p1, Point p2);
Point vecNormalize(Point p);
Point vecSubtract(Point p1, Point p2);
Point vecMultiply(Point p, float scalar);
Point crossProduct(Point p1, Point p2);
#endif // GEOMETRY_H