#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "3dobjects.h"
#include <string.h>

typedef Object** Scene;

Scene RTnewScene(int c, ...){
    va_list ptr;
    va_start(ptr, c);
    Scene scene = malloc(sizeof(Object*) * (c+1));
    for(int i = 0; i < c; i++)
        scene[i] = va_arg(ptr, Object*);
    scene[c] = NULL;
    va_end(ptr);
    return scene;
}

int Scene_append(Scene *scene, Object *object){
    Scene current = *scene;
    int len = 0;
    for(;current[len] != NULL; len++);
    Scene new_scene = malloc(sizeof(Object*) * (len+2));
    memcpy(new_scene, current, sizeof(Object*) * len);
    free(current);
    new_scene[len] = object;
    new_scene[len+1] = NULL;
    *scene = new_scene;
    return 0;
}

int Scene_remove(Scene *scene, Object *object){
    Scene current = *scene;
    int len = 0;
    for(;current[len] != NULL; len++);
    for(int i = 0; current[i] != NULL; i++){
        if(current[i] == object){
            memcpy(&current[i], &current[i+1], sizeof(Object*)*(len-i));
            current = realloc(current, sizeof(Object*)*(len));
            current[len-1] = NULL;
            *scene = current;
            return 0;
        }
    }
    return -1;
}

int Scene_removeAt(Scene *scene, int index){
    Scene current = *scene;
    int len = 0;
    for(;current[len] != NULL; len++);len++;
    if(index > len)
        return -1;
    memcpy(&current[index], &current[index+1], sizeof(Object*)*(len-index));
    current = realloc(current, sizeof(Object*)*(len));
    current[len-1] = NULL;
    *scene = current;
    return 0;
}

#endif // SCENE_H_INCLUDED
