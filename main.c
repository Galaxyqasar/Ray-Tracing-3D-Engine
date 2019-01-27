#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <stdbool.h>
#include <pthread.h>

#define WINDOWS

#include "eventSystem.h"
#include "mouse.h"
#include "inputSystem.h"
#include "header.h"

#include "RayTracer/rayTracer.h"
#include "Physics/physics.h"

const double pi = 3.141592654;
const double angle = 0.3141592654 / 5;

const Vector3 O = (Vector3){0.0,0.0,0.0};
const Vector3 X = (Vector3){1.0,0.0,0.0};
const Vector3 Y = (Vector3){0.0,1.0,0.0};
const Vector3 Z = (Vector3){0.0,0.0,1.0};

int s_testSlot = -1;
int s_render = -1;
int moves = 0, clicks = 0;
Vector2 oldMousePos = (Vector2){1,1}, mousePos = (Vector2){1,1};
int oldChar = ' ';

pthread_t *renderThreads;

bool stopRender = false;
////////////////////////////////////////////////
Camera cam;
Vector3 campos;
Vector3 lookAt;
////////////////////////////////////////////////

void testSlot(SlotArgs *args){
    printw(args->format);
    refresh();
}

void keyPressed(SlotArgs *args){
    int c = *(int*)args->data;
    if(c == '\n')
        App.quit();
    else if(c == 'x')
        emit(s_testSlot, &(SlotArgs){"Hello world", NULL});
    else if(c == 'w'){               /// Movement
        Vector3 myPoint = lookAt;
        Vector3 myOrigin = campos;
        Vector3 amnt = *Vector3_add(&myPoint, Vector3_invert(&myOrigin));
        Vector3_multiplie(&amnt, 1);
        amnt.y = 0;
        Vector3_add(&campos, &amnt);
        Vector3_add(&lookAt, &amnt);
    }
    else if(c == 'a'){
        Vector3 myPoint = lookAt;
        Vector3 myOrigin = campos;
        Vector3 amnt = *Vector3_add(&myPoint, Vector3_invert(&myOrigin));
        Vector3_Rotate(&amnt, &O, pi/2.0, 0.0);
        Vector3_multiplie(&amnt, -1);
        amnt.y = 0;
        Vector3_add(&campos, &amnt);
        Vector3_add(&lookAt, &amnt);
    }
    else if(c == 's'){
        Vector3 myPoint = lookAt;
        Vector3 myOrigin = campos;
        Vector3 amnt = *Vector3_add(&myPoint, Vector3_invert(&myOrigin));
        Vector3_multiplie(&amnt, -1);
        amnt.y = 0;
        Vector3_add(&campos, &amnt);
        Vector3_add(&lookAt, &amnt);
    }
    else if(c == 'd'){
        Vector3 myPoint = lookAt;
        Vector3 myOrigin = campos;
        Vector3 amnt = *Vector3_add(&myPoint, Vector3_invert(&myOrigin));
        Vector3_Rotate(&amnt, &O, pi/2.0, 0.0);
        Vector3_multiplie(&amnt, 1);
        amnt.y = 0;
        Vector3_add(&campos, &amnt);
        Vector3_add(&lookAt, &amnt);
    }

    else if(c == KEY_LEFT){     /// Rotation
        Vector3_Rotate(&lookAt, &campos, -angle, 0.0);
    }
    else if(c == KEY_RIGHT){
        Vector3_Rotate(&lookAt, &campos, angle, 0.0);
    }
    else if(c == KEY_UP){
        Vector3_Rotate(&lookAt, &campos, 0.0, angle);
    }
    else if(c == KEY_DOWN){
        Vector3_Rotate(&lookAt, &campos, 0.0, -angle);
    }

    else if(c == 'q'){
        campos.y += 0.1;
        lookAt.y += 0.1;
    }
    else if(c == 'e'){
        campos.y -= 0.1;
        lookAt.y -= 0.1;
    }
    //else
        //mvprintw(0,0,"key pressed\t: \'%c\'  ", c);

    cam = newCamera(campos, lookAt);
    cleanSlotArgs(args);
}

void mouseMoved(SlotArgs *args){
    MouseEvent *event = (MouseEvent*)args->data;
    //mvprintw(1,0,"%s\t: %4i\t\t%3i\t(%3i|%3i), (%3i|%3i) ", args->format, moves, event->type, event->x, event->y, event->dx, event->dy);
    mousePos.x = event->x-1;
    mousePos.y = event->y-1;/*
    mvprintw(oldMousePos.y, oldMousePos.x, "%c", oldChar);
    oldChar = mvinch(mousePos.y, mousePos.x);
    mvprintw(mousePos.y, mousePos.x, "X");
    refresh();*/
    moves++;
    oldMousePos = mousePos;
    cleanSlotArgs(args);

    Vector3_Rotate(&lookAt, &campos, angle* event->dx, -1 * angle * event->dy);
    cam = newCamera(campos, lookAt);
}

void mouseClicked(SlotArgs *args){
    MouseEvent *event = (MouseEvent*)args->data;
    mvprintw(2,0,"%s\t: %4i\t\t%3i\t(%3i|%3i) ", args->format, clicks, event->type, event->x, event->y);
    refresh();
    clicks++;
    cleanSlotArgs(args);
}

void* render(void *data){
    Rect *rect = data;
    int x = rect->x, y = rect->y, dx = rect->dx, dy = rect->dy;
    clock_t start = clock();
    while(!stopRender){
        RTrender(x,y,dx,dy);
        RTshow();
        mvprintw(0,0,"%f fps", 1000.0/((double)clock() - (double)start));
        start = clock();
    }
    return NULL;
}

////////////////////////////////////

int cleanUpFn(){
    stopMouse = true;
    stopRender = true;
    stopPhysics = true;
    cleanMouse();
    endwin();
    RTclean();
    printf("cleaned up\n");
    return 0;
}

int initCurses(){
    initscr();
    cbreak();
    noecho();
    timeout(1);
    curs_set(0);
    keypad(stdscr, true);
    return 0;
}

int main(){
    printf("start setup\n");
    srand((unsigned)time(NULL));
    initCurses();
    App.init(cleanUpFn);
    InputSystem.init(keyPressed, mouseMoved, mouseClicked);
    int w, h;
    getmaxyx(stdscr, h,w);
    RTinit(w-2, (h*2)-2, 1, 2, 1, 0.2, 0.0000001);

    Color tileFloor = newColor(0.125, 0.125, 0.125, 2.0, 0.0);
    Color maroon = newColor(0.5, 0.25, 0.25, 1.0, 0.0);
    Color prettyGreen = newColor(0.5, 1.0, 0.5, 0.5, 0.0);

    Vector3 P1 = newVector3(0.0, 2.0, 0.0);
    Vector3 P2 = newVector3(0.0, -1.0, 0.0);
    Vector3 P3 = newVector3(-0.0, 5.0, -0.0);
    Vector3 P4 = newVector3(5.0, 3.0, 0.0);
    campos = newVector3(5.0, 2.0, 0.0);
    lookAt = newVector3(campos.x - 0.25, campos.y, campos.z);

    Light light = newLight(newVector3(0.0, 10.0, 0.0), newColor(1.0, 1.0, 1.0, 0.0, 0.0));

    Object sphere  = newObject(Sphere_t, prettyGreen,   P3, newVector3(0.0, 0.0, 0.0), noAcceleration, 1.0, active, newSphere(1));
    Object rectangle = newObject(Rectangle_t, newColor(0.0,1.0,0.0,0.0,0.0), P1, noSpeed, noAcceleration, 1.0, passive,
                                newRectangle(newVector3(0,0,-2), newVector3(0,4,-2), newVector3(0,4,2), newVector3(0,0,2), true, loadImage("text2.bmp")));
    Object sphere2 = newObject(Sphere_t, maroon,        P1, newVector3(0.0,5.0,0.0), noAcceleration, 10.0, ignore, newSphere(0.5));
    Object sphere3 = newObject(Sphere_t, maroon,        P4, newVector3(5.0,5.0,5.0), noAcceleration, 10.0, passive, newSphere(0.5));
    Object plane   = newObject(Plane_t , tileFloor,     P2, noSpeed, noAcceleration, passive, 1.0, newPlane(newVector3(0.0,1.0,0.0), -1));

    cam = newCamera(campos, lookAt);
    Scene scene = RTnewScene(1, &plane);
    Scene_append(&scene, &sphere);
    Scene_append(&scene, &sphere2);
    Scene_append(&scene, &rectangle);
    //Scene_removeAt(&scene, 0);
    //loadLevel("level.dat", &scene, NULL, NULL);

    Lights lights = RTnewLights(1, &light);
    RTsetScene(&cam, scene, lights);
    Pinit(scene, 0.0);

    int threads = 1;
    renderThreads = malloc(sizeof(pthread_t) * threads);
    Rect rects[] = {(Rect){0,0,1,1}, (Rect){1,0,2,2}, (Rect){0,1,2,2}, (Rect){1,1,2,2}};
    for(int i = 0; i < threads; i++)
        if(pthread_create(&renderThreads[i], NULL, render, &rects[i]) != 0)
            printw("error");
    printf("end setup\n");
    saveImage(loadImage("texture.bmp"), "test2.bmp");
    return App.exec();
}

