#ifndef COLLISION_H
#define COLLISION_H

typedef struct point{
    int x;
    int y;
} Point;
typedef struct rect{
    Point p1;
    Point p2;
} Rectangle;
int AABB(Rectangle r1, Rectangle r2);

#endif