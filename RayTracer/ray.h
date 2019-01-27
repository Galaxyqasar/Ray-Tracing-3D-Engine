#ifndef RAY_H
#define RAY_H

#include "vector.h"

typedef struct Ray {
    Vector3 origin, direction;
} Ray;

Ray newRay(Vector3 o, Vector3 d){
    return (Ray){o,d};
}

#endif // RAY_H






