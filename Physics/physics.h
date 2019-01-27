#include <pthread.h>

#include "../RayTracer/vector.h"
#include "../RayTracer/3dobjects.h"

Scene Pscene;
pthread_t physicsThread;
bool stopPhysics = false;
clock_t PstartTime;
unsigned dt = 10;
double t = 0.0;
double s = 1000.0;
double PglobalFriction;
int msecsLeft = 0;

enum Behavior{
    ignore,
    passive,
    active
};

const Vector3 noSpeed = (Vector3){0.0,0.0,0.0};
const Vector3 noAcceleration = (Vector3){0.0,0.0,0.0};
const Vector3 g = (Vector3){0.0,-9.81,0.0};

void* calcPhysics(void *data);

int Pinit(Scene scene, double globalFriction){
    Pscene = scene;
    PglobalFriction = globalFriction;
    pthread_create(&physicsThread, NULL, calcPhysics, NULL);
}

void* calcPhysics(void *data){
    while(!stopPhysics){
        msecsLeft = 0;
        t += dt/s;
        PstartTime = clock();
        for(int i = 0; Pscene[i] != NULL; i++){
            Object *current = Pscene[i];
            if(current->behavior != active)
                continue;
            Vector3 v = current->v;
            Vector3 a = current->a;
            Vector3_add(&a, &g);
            Vector3_add(&v, Vector3_multiplie(&a, dt/s));
            Vector3_multiplie(&v, 1.0 - PglobalFriction * (dt/s));    //friction
            current->v = v;
            Vector3_add(&current->pos, Vector3_multiplie(&v, dt/s));
            for(int k = 0; Pscene[k] != NULL; k++){
                if(i == k)
                    continue;
                Object *target = Pscene[k];
                if(target->behavior == ignore)
                    continue;
                //check for collision;
                if(target->type == Plane_t){
                    if(current->type == Sphere_t){
                        Sphere *sphere = current->data;
                        if(current->pos.y - sphere->radius > target->pos.y)
                            continue;
                        else if(current->pos.y + sphere->radius < target->pos.y)
                            continue;
                        else{
                            current->v.y *= -0.75;
                            if(current->pos.y > target->pos.y)
                                current->pos.y = sphere->radius + target->pos.y;
                            if(current->pos.y < target->pos.y)
                                current->pos.y = -(sphere->radius + target->pos.y);
                        }
                    }
                }
                else if(target->type == Sphere_t){
                    if(current->type == Sphere_t){
                        Sphere *sphere1 = current->data;
                        Sphere *sphere2 = target->data;
                        double distx = target->pos.x - current->pos.x;
                        double disty = target->pos.y - current->pos.y;
                        double distz = target->pos.z - current->pos.z;
                        double dist = sqrt(distx*distx + disty*disty + distz*distz);
                        double targetDist = sphere1->radius + sphere2->radius;
                        if(dist > targetDist)
                            continue;
                        //current->v.y *= -0.75;
                        //current->v.x -= 0.02;
                        //current->v.z -= 0.02;
                        //current->pos.y += 0.02;//target->pos.y + targetDist;
                        //continue;
                        //if(current->pos.x == target->pos.x)
                        //double totalV = Vector3_magnitude(&current->v);
                        double ax, ay, az;
                        Vector3_getAngle(&target->pos, &current->pos, &ax, &ay, &az);
                        //current->v = Vector3_fromLine(totalV, ax, ay, az);
                        Vector3 d = Vector3_fromLine(targetDist, ax, ay, az);
                        //if(Vector3_magnitude(&d) > targetDist+0.01)
                        current->pos = *Vector3_add(&d, &target->pos);
                        //Vector3_invert(&current->v);
                        //current->v.y *= 0.5;
                        Vector3_invert(&current->v);
                    }
                    else
                        continue;
                }
                else
                    continue;
            }
        }
        while(clock() < (PstartTime + dt)){
            usleep(1000);
        }
    }
}
