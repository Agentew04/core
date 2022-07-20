#include "collision.h"

int AABB(Rectangle r1, Rectangle r2){
    if(r1.p1.x < r2.p2.x &&
        r1.p2.x > r2.p1.x &&
        r1.p1.y < r2.p2.y &&
        r1.p2.y > r2.p1.y)
        return 1;
    return 0;
}