#include "geometry.h"
#include <math.h>

float getRadiusFromVolume(float volume){
    // r = raiz(area/pi)
    return sqrt(volume/M_PI);
}
float getAngleBetweenPoints(Point p1, Point p2){
    double rad = atan2(p1.y-p2.y, p2.x - p1.x);
    return rad;
}
int checkCircleArcCollision(Arc a, Circle c){
    float circleAlpha = getAngleBetweenPoints(a.center, c.center);
    float distArcCircle = getDistance(a.center, c.center)-(c.radius+a.radius+a.thickness);
    // check if circleAlpha is between startAlpha and startAlpha+deltaAlpha
    if(circleAlpha >= a.startAlpha && circleAlpha <= a.startAlpha+a.deltaAlpha){
        return distArcCircle <= 0;
    }
    return 0;
}
int checkCircleCicleCollision(Point c1, float r1, Point c2, float r2){
    float dist = getDistance(c1, c2);
    if(dist<r1+r2)
        return 1;
    return 0;
}
float getDistance(Point p1, Point p2){
    return sqrt(pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2));
}
