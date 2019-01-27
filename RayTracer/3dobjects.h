#ifndef _3DOBJECTS_H
#define _3DOBJECTS_H

#include <math.h>
#include <stdarg.h>

#include "vector.h"
#include "color.h"
#include "ray.h"
#include "image.h"

typedef enum ObjectType{
    Object_t,
    Plane_t,
    Triangle_t,
    Rectangle_t,
    Polygon_t,
    Sphere_t,
    Group_t
} ObjectType;

typedef struct Object {
    ObjectType type;
    Color color;
    Vector3 pos, v, a;
    float m;
    int behavior;
    void *data;
} Object;

typedef struct Plane{
    Vector3 normal;
    double distance;
} Plane;

typedef struct Triangle{
    Vector3 *points;
} Triangle;

typedef struct Rectangle{
    Vector3 *points;
    bool textured;
    Image texture;
} Rectangle;

typedef struct Polygon{
    int count;
    Vector3 *points;
} Polygon;

typedef struct Sphere{
    double radius;
} Sphere;

typedef Object** Group;

Object newObject(ObjectType type, Color color, Vector3 position, Vector3 speed, Vector3 acceleration, double mass, int physics, void *data){
    return (Object){type, color, position, speed, acceleration, mass, physics, data};
}

void* newPolygon(int count, ...);

void* newPlane(Vector3 normal, double distance){
    void *plane = malloc(sizeof(Plane));
    Plane tmp = (Plane){normal, distance};
    memcpy(plane, &tmp, sizeof(Plane));
    return plane;
}

void* newSphere(double radius){
    void *sphere = malloc(sizeof(Sphere));
    Sphere tmp = (Sphere){radius};
    memcpy(sphere, &tmp, sizeof(Sphere));
    return sphere;
}

void* newTriangle(Vector3 A, Vector3 B, Vector3 C){
    void *triangle = malloc(sizeof(Triangle));
    Triangle tmp = (Triangle){((Polygon*)newPolygon(3, A, B, C))->points};
    memcpy(triangle, &tmp, sizeof(Triangle));
    return triangle;
}

void* newRectangle(Vector3 A, Vector3 B, Vector3 C, Vector3 D, bool textured, Image texture){
    void *rect = malloc(sizeof(Rectangle));
    Rectangle tmp = (Rectangle){((Polygon*)newPolygon(4, A, B, C, D))->points, textured, texture};
    memcpy(rect, &tmp, sizeof(Rectangle));
    return rect;
}

void* newPolygon(int count, ...){
    va_list ptr;
    va_start(ptr, count);
    Vector3 *points = malloc(sizeof(Vector3)*count);
    for(int i = 0; i < count; i++)
        points[i] = va_arg(ptr, Vector3);
    va_end(ptr);

    void *polygon = malloc(sizeof(Polygon));
    Polygon tmp = (Polygon){count, points};
    memcpy(polygon, &tmp, sizeof(Polygon));
    return polygon;
}

Vector3 Plane_getNormal(Plane *plane);
Vector3 Triangle_getNormal(Triangle *triangle);
Vector3 Rectangle_getNormal(Rectangle *rectangle);
Vector3 Polygon_getNormal(Polygon *polygon);
Vector3 Sphere_getNormalAt(Object *object, Vector3 intersection_position);

double Plane_findIntersection(Plane *plane, Ray ray);
double Triangle_findIntersection(Triangle *triangle, Ray ray);
double Rectangle_findIntersection(Rectangle *rectangle, Ray ray);
double Polygon_findIntersection(Polygon *polygon, Ray ray);
double Sphere_findIntersection(Object *Object, Ray ray);

Color Rectangle_getColorAt(Rectangle *rect, Vector3 point, Color objectColor);

Color Object_getColor(Object *object, Vector3 point){
    if(object->type == Object_t)
        return newColor(0.0,0.0,0.0,0.0,0.0);
    else if(object->type == Rectangle_t){
        Rectangle *rect = object->data;
        if(rect->textured)
            return Rectangle_getColorAt(rect, point, object->color);
        else
            return object->color;
    }
    return object->color;
}

Vector3 Object_getNormalAt(Object *object, Vector3 intersection_position) {
    if(object->type == Object_t)
        return newVector3 (0, 0, 0);
    else if(object->type == Plane_t){
        return Plane_getNormal(object->data);
    }
    else if(object->type == Triangle_t){
        return Triangle_getNormal(object->data);
    }
    else if(object->type == Rectangle_t){
        return Rectangle_getNormal(object->data);
    }
    else if(object->type == Polygon_t){
        return Polygon_getNormal(object->data);
    }
    else if(object->type == Sphere_t){
        return Sphere_getNormalAt(object, intersection_position);
    }
    return newVector3 (0, 0, 0);
}

double Object_findIntersection(Object *object, Ray ray) {
    if(object->type == Object_t)
        return 0;
    else if(object->type == Plane_t){
        return Plane_findIntersection(object->data, ray);
    }
    else if(object->type == Triangle_t){
        return Triangle_findIntersection(object->data, ray);
    }
    else if(object->type == Rectangle_t){
        return Rectangle_findIntersection(object->data, ray);
    }
    else if(object->type == Polygon_t){
        return Polygon_findIntersection(object->data, ray);
    }
    else if(object->type == Sphere_t){
        return Sphere_findIntersection(object, ray);
    }
    return 0.0;
}

/// Get normal

Vector3 Plane_getNormal(Plane *plane){
    return plane->normal;
}

Vector3 Triangle_getNormal(Triangle *triangle){
    Polygon polygon = (Polygon){3, triangle->points};
    return Polygon_getNormal(&polygon);
}

Vector3 Rectangle_getNormal(Rectangle *rectangle){
    Polygon polygon = (Polygon){4, rectangle->points};
    return Polygon_getNormal(&polygon);
}

Vector3 Polygon_getNormal(Polygon *polygon){
    if(polygon->count < 3)
        return newVector3 (0, 0, 0);
    Vector3 A = polygon->points[0], B = polygon->points[1], C = polygon->points[2];
    Vector3 CA = *Vector3_add(&C, Vector3_invert(&A));
    Vector3_invert(&A);
    Vector3 BA = *Vector3_add(&B, Vector3_invert(&A));
    return *Vector3_normalize(Vector3_crossProduct(&CA, &BA));
}

Vector3 Sphere_getNormalAt(Object *object, Vector3 intersection_position){
    Vector3 ncenter = object->pos;
    Vector3_normalize(Vector3_add(&intersection_position, Vector3_invert(&ncenter)));
    return intersection_position;
}

/// Find intersection

double Plane_findIntersection(Plane *plane, Ray ray){
    Vector3 ray_direction = ray.direction;
    Vector3 normal = plane->normal;
    double a = Vector3_dotProduct(&ray_direction, &normal);
    if (a == 0) {
        return -1.0;
    }
    else {
        Vector3 ray_origin = ray.origin;
        double b = Vector3_dotProduct(&normal, Vector3_add(&ray_origin, Vector3_invert(Vector3_multiplie(&normal, plane->distance))));
        return -1*b/a;
    }
}

double Triangle_findIntersection(Triangle *triangle, Ray ray){
    Polygon polygon = (Polygon){3, triangle->points};
    return Polygon_findIntersection(&polygon, ray);
}
double Rectangle_findIntersection(Rectangle *rectangle, Ray ray){
    Polygon polygon = (Polygon){4, rectangle->points};
    return Polygon_findIntersection(&polygon, ray);
}

double Polygon_findIntersection(Polygon *polygon, Ray ray){
    Vector3 normal = Polygon_getNormal(polygon);
    double distance = Vector3_dotProduct(&normal, &polygon->points[0]);
    Vector3 ray_origin = ray.origin;
    Vector3 ray_direction = ray.direction;

    double a = Vector3_dotProduct(&ray_direction, &normal);
    if (a == 0)
        return -1.0;
    Vector3 tmpnormal = normal;
    double b = Vector3_dotProduct(&normal, Vector3_add(&ray_origin, Vector3_invert(Vector3_multiplie(&tmpnormal, distance))));
    normal = Polygon_getNormal(polygon);
    double dist2plane = -1*b/a;
    ray_origin = ray.origin;
    Vector3 Q = *Vector3_add(Vector3_multiplie(&ray_direction, dist2plane), &ray_origin), tmpQ = Q;

    for(int i = 0; i < polygon->count; i++){
        Vector3 A = polygon->points[i], D = polygon->points[(i == polygon->count-1)? 0:i+1];
        Vector3 current = *Vector3_add(&D, Vector3_invert(&A));
        D = polygon->points[(i == polygon->count-1)? 0:i+1];   A = polygon->points[i];
        Vector3 Qcurrent = *Vector3_add(&tmpQ, Vector3_invert(&A));   tmpQ = Q;
        double test = Vector3_dotProduct(Vector3_crossProduct(&current, &Qcurrent), &normal);
        if(test > 0.0)
            return -1.0;
    }
    return -1*b/a;
}

double Sphere_findIntersection(Object *object, Ray ray){
    Sphere *sphere = object->data;
    Vector3 ray_origin = ray.origin;
    double ray_origin_x = ray_origin.x;
    double ray_origin_y = ray_origin.y;
    double ray_origin_z = ray_origin.z;

    Vector3 ray_direction = ray.direction;

    Vector3 sphere_center =object->pos;
    double sphere_center_x = sphere_center.x;
    double sphere_center_y = sphere_center.y;
    double sphere_center_z = sphere_center.z;

    double radius = sphere->radius;
    double b = (2*(ray_origin_x - sphere_center_x)*ray_direction.x) + (2*(ray_origin_y - sphere_center_y)*ray_direction.y) + (2*(ray_origin_z - sphere_center_z)*ray_direction.z);
    double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);

    double discriminant = b*b - 4*c;

    if (discriminant > 0) { // the ray intersects the sphere
        // the first root
        double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;

        if (root_1 > 0) // the first root is the smallest positive root
            return root_1;
        else // the second root is the smallest positive root
            return ((sqrt(discriminant) - b)/2) - 0.000001;
    }
	return -1;
}

Color Rectangle_getColorAt(Rectangle *rect, Vector3 point, Color objectColor){
    double x, y;    ///coordinates in texture in 2d plane
    Vector3 A = rect->points[0], C = rect->points[2], tmp = *Vector3_add(&A, Vector3_invert(&point)); Vector3_invert(&point);
    A = rect->points[0];
    double height = C.y - A.y;
    A.y = 0; C.y = 0; tmp.y = 0;
    double width = Vector3_magnitude(&A) + Vector3_magnitude(&C);
    x = ((Vector3_magnitude(&tmp))/width)*rect->texture.width;
    y = rect->texture.height - (height - point.y)/height*rect->texture.height;
    //x = (point.z /4 + 0.5) * rect->texture.width;
    int thisone = (int)y  * rect->texture.width + (int)x;
    Color color = Color_fromRGB(rect->texture.data[thisone]);
    color.a = objectColor.a;
    color.s = objectColor.s;
    return color;
}

#endif // _3DOBJECTS_H
