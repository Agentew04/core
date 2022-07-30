#include "geometry.h"
#include <math.h>

float getRadiusFromVolume(float volume){
    // r = raiz(area/pi)
    return sqrt(volume/M_PI);
}
float getVolumeFromRadius(float radius){
    // area = pi*r^2
    return M_PI*radius*radius;
}

float getAngleBetweenPoints(Point p1, Point p2){
    // -180 <= alpha <= 180
    double rad = atan2(p1.y-p2.y, p2.x - p1.x);
    return rad;
}

int isAngleBetween(float angle, float a, float b){
    // a and b are -pi and pi
    if(a<0)
        a += 2*M_PI;
    if(b<0)
        b += 2*M_PI;
    if(a>b)
        return angle>a || angle<b;
    else
        return angle>a && angle<b;
}

int checkCircleArcCollision(Arc a, Circle c){
    float circleAlpha = getAngleBetweenPoints(a.center, c.center);
    float distArcCircle = getDistance(a.center, c.center)-(c.radius+a.radius+a.thickness);
    printf("dist = %f circleAlpha=%f\n", distArcCircle, circleAlpha);

    int isBetween = isAngleBetween(circleAlpha, a.startAlpha, a.startAlpha+a.deltaAlpha);
    if(isBetween){
        return distArcCircle <= 0 && distArcCircle >= -a.thickness;
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
