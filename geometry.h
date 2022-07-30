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

float getVolumeFromRadius(float radius);
float getRadiusFromVolume(float volume);
int isAngleBetween(float angle, float a, float b);
float getAngleBetweenPoints(Point p1, Point p2);
float getDistance(Point p1, Point p2);
int checkCircleArcCollision(Arc a, Circle c);
int checkCircleCicleCollision(Point c1, float r1, Point c2, float r2);

#endif // GEOMETRY_H