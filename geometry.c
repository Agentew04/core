#include "geometry.h"
#include <math.h>
#include <stdio.h>

float getDistance(Point p1, Point p2){
    return sqrt(pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2));
}
float getAngleBetweenPoints(Point p1, Point p2){
    double rad = atan2(p1.y-p2.y, p2.x - p1.x);
    return rad;
}  
float toDegrees(float rad){
    return rad*180/M_PI;
}
float toRadians(float deg){
    return deg*M_PI/180;
}
float getRadiusFromVolume(float volume){
    return sqrt(volume/M_PI);
}
float getVolumeFromRadius(float radius){
    return M_PI*radius*radius;
}
int isAngleBetween(float start, float end, float mid){
    end = (end - start) < 0.0f ? end - start + 360.0 : end - start;    
    mid = (mid - start) < 0.0f ? mid - start + 360.0 : mid - start; 
    return (mid < end);
}
float vecLength(Point p){
    return sqrt(p.x*p.x + p.y*p.y);
}
float vecAngle(Point p){
    return atan2(p.y, p.x);
}
Point vecFromAngleLength(float angle, float length){
    Point p;
    p.x = cos(angle)*length;
    p.y = sin(angle)*length;
    return p;
}
Point vecFromPoints(Point p1, Point p2){
    Point p;
    p.x = p2.x - p1.x;
    p.y = p2.y - p1.y;
    return p;
}
float dotProduct(Point p1, Point p2){
    return p1.x*p2.x + p1.y*p2.y;
}
Point vecNormalize(Point p){
    float length = vecLength(p);
    Point pn;
    pn.x = p.x/length;
    pn.y = p.y/length;
    return pn;
}
Point vecSubtract(Point p1, Point p2){
    Point p;
    p.x = p1.x - p2.x;
    p.y = p1.y - p2.y;
    return p;
}
Point crossProduct(Point p1, Point p2){
    Point p;
    p.x = p1.x*p2.y - p1.y*p2.x;
    p.y = p1.x*p2.x + p1.y*p2.y;
    return p;
}
Point vecMultiply(Point p, float scalar){
    Point pn;
    pn.x = p.x*scalar;
    pn.y = p.y*scalar;
    return pn;
}

int checkCircleArcCollision(Arc a, Circle c){
    // get if touching
    float dist = getDistance(a.center, c.center);
    int isInPerimeter = dist <= a.radius && dist >= a.radius-a.thickness;

    // get if inside range
    float angle = getAngleBetweenPoints(c.center, a.center);
    float start = a.startAlpha;
    float end = a.startAlpha + a.deltaAlpha;
    int isInRange = isAngleBetween(toDegrees(start), toDegrees(end), toDegrees(angle));
    return isInPerimeter && isInRange;
}
int checkCircleCicleCollision(Circle c1, Circle c2){
    float dist = getDistance(c1.center, c2.center);
    if(dist<=c1.radius+c2.radius)
        return 1;
    return 0;
}
int pointInsideRect(Point p, Rectangle r){
    return p.x >= r.p1.x && p.x <= r.p2.x && p.y >= r.p1.y && p.y <= r.p2.y;
}