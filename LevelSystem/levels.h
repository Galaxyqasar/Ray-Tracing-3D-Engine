#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

#include <stdio.h>

#include "../RayTracer/3dobjects.h"
#include "../RayTracer/scene.h"
#include "../RayTracer/light.h"

int loadLevel(char *filename, Scene *RTscene, Scene* Pscene, Lights *lights){
    FILE *f = fopen(filename, "r");
    int objectCount;
    fread(&objectCount, 1, 4, f);
    char tmp;
    fread(&tmp, 1, 1, f);
    if(tmp != '|')  return -1;
    Vector3 playerPos;
    fread(&playerPos, 3, 8, f);
    fread(&tmp, 1, 1, f);
    if(tmp != '{')  return -1;
    fread(&tmp, 1, 1, f);

    for(int i = 0; i < objectCount; i++){
        char cType;
        fread(&cType, 1, 1, f);
        void *data = NULL;
        int type = Object_t, physics = ignore;
        Color color;
        Vector3 pos, v, a;
        double mass = 0.0;
        /// get properties
        switch(cType){
            case 'F':{
                type = Plane_t;
                Vector3 normal;
                double distance;
                /// get properties
                data = newPlane(normal, distance);
            } break;
            case 'T':{
                type = Triangle_t;
                Vector3 points[3];
                /// get properties
            } break;
            case 'R':{
                type = Rectangle_t;
                /// get properties
            } break;
            case 'P':{
                type = Polygon_t;
                /// get properties
            } break;
            case 'S':{
                type = Sphere_t;
                /// get properties
            } break;
            case 'G':{
                type = Group_t;
                /// get properties
            } break;
            default:{
                ;
            }
        }
        Object *current = newObject(type, color, pos, v, a, mass, physics, data);
        Scene_append(RTscene, current);
    }
    fclose(f);
    return 0;
}

int saveLevel(char *filename, Scene *RTscene, Scene* Pscene, Lights *lights){
    ;
}

#endif // LEVELS_H_INCLUDED
