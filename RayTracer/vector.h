#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct Vector3{
    double x, y, z;
} Vector3;

Vector3 newVector3(double x, double y, double z){
    return (Vector3){x,y,z};
};

double Vector3_magnitude(Vector3 *v){
    return sqrt((v->x*v->x)+(v->y*v->y)+(v->z*v->z));
}
Vector3* Vector3_normalize(Vector3 *v){
    double mag = Vector3_magnitude(v);
    if(mag != 0){
        v->x /= mag;
        v->y /= mag;
        v->z /= mag;
    }
    return v;
}
Vector3* Vector3_invert(Vector3 *v){
    v->x*=-1;
    v->y*=-1;
    v->z*=-1;
    return v;
}
double Vector3_dotProduct(Vector3 *v, Vector3 *a){
    return v->x*a->x+v->y*a->y+v->z*a->z;
}
Vector3* Vector3_crossProduct(Vector3 *v, Vector3 *a){
    Vector3 tmp = newVector3(v->y*a->z - v->z*a->y, \
                              v->z*a->x - v->x*a->z, \
                              v->x*a->y - v->y*a->x);
    memcpy(v, &tmp, sizeof(Vector3));
    return v;
}
Vector3* Vector3_add(Vector3 *v, Vector3 *a){
    v->x += a->x;
    v->y += a->y;
    v->z += a->z;
    return v;
}
Vector3* Vector3_multiplie(Vector3 *v, double scalar){
    v->x*=scalar;
    v->y*=scalar;
    v->z*=scalar;
    return v;
}

Vector3 *Vector3_Rotate(Vector3 *point, Vector3 *origin, float ax, float ay){
    Vector3 myPoint = *point;
    Vector3 myOrigin = *origin;
    Vector3 translated = *Vector3_add(&myPoint, Vector3_invert(&myOrigin));
    if(ax != 0){
        translated = newVector3(translated.x * cos(ax) + translated.z * sin(ax),
                                translated.y,
                                -translated.x * sin(ax) + translated.z * cos(ax));
    }
    if(ay != 0){
        double distxz = sqrt(translated.x * translated.x + translated.z * translated.z);
        double dist = sqrt(distxz * distxz + translated.y * translated.y);

        double newA = sinh(translated.y / dist) + ay;
        double newDistxz = cos(newA)*dist;
        double height = sin(newA) * dist;
        double newY = dist * (height / dist);
        double newX = (translated.x / distxz) * newDistxz;
        double newZ = (translated.z / distxz) * newDistxz;

        translated = newVector3(newX, newY, newZ);

    }
    myPoint = *Vector3_add(&translated, Vector3_invert(&myOrigin));
    *point = myPoint;
    return point;
}

void Vector3_getAngle(Vector3 *v, Vector3 *o, double *ax, double *ay, double *az){
    Vector3 pos = *v, origin = *o;
    Vector3 d = *Vector3_add(&pos, Vector3_invert(&origin));
    double distxz = sqrt(d.x*d.x + d.z*d.z);
    double dist = Vector3_magnitude(&d);
    *ax = sinh(d.x /distxz);
    *ay = cosh(distxz/dist);
    *az = sinh(d.z /distxz);
}

Vector3 Vector3_fromLine(double dist, double ax, double ay, double az){
    double distxz = cos(ay)*dist;
    double x = sin(ax)*distxz;
    double y = sin(ay)*dist;
    double z = sin(az)*distxz;
    return newVector3(x,y,z);
}

#endif // VECTOR_H



