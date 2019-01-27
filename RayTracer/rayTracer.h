#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <stdarg.h>

#include "ray.h"
#include "light.h"
#include "color.h"
#include "vector.h"
#include "camera.h"
#include "functions.h"
#include "3dobjects.h"
#include "image.h"
#include "scene.h"
typedef struct Rect{
    int x, y, dx, dy;
} Rect;

int aadepth;
int width;
int height;
int n;
int xStep, yStep;
float ambientlight;
float accuracy;
float aspectratio;

RGBType *pixels;
float *fpixels;

int thisone, aaIndex;
float xamnt, yamnt;

Camera *RTcam;
Scene RTscene;
Lights RTlightes;

void RTinit(int n_width, int n_height, int n_xStep, int n_yStep, int n_aadepth, float n_ambientlight, float n_accuracy){
    width = n_width;
    height = n_height;
    xStep = n_xStep, yStep = n_yStep;
    aadepth = n_aadepth;
    ambientlight = n_ambientlight;
    accuracy = n_accuracy;

    n = width*height+1;
    aspectratio = (float)width/(float)height;
    pixels = malloc(sizeof(RGBType)*n);
    fpixels = malloc(sizeof(float)*n);
}

void RTclean(){
    savebmp("test.bmp", width, height, 72, pixels);
    free(pixels);
    free(fpixels);
}

Lights RTnewLights(int c, ...){
    va_list ptr;
    va_start(ptr, c);
    Lights lights = malloc(sizeof(Light*) * (c+1));
    for(int i = 0; i < c; i++)
        lights[i] = va_arg(ptr, Light*);
    lights[c] = NULL;
    va_end(ptr);
    return lights;
}

void RTsetScene(Camera *cam, Scene newScene, Lights newLights){
    RTcam = cam;
    RTscene = newScene;
    RTlightes = newLights;
}

void RTrender(int xoffset, int yoffset, int xscale, int yscale){
    int len = 0;
    for(;RTscene[len] != NULL; len++);
    float *intersections = malloc(sizeof(float)*len);
    int s_aadepth = aadepth*aadepth;
    RGBType *tempRGB = malloc(sizeof(RGBType)*s_aadepth);

    for (int x = xoffset; x < width; x+=xStep*xscale) {
        for (int y = yoffset; y < height; y+=yStep*yscale) {
            thisone = (y*width + x);

            // create the ray from the camera to this pixel
            for (int aax = 0; aax < aadepth; aax++) {
                for (int aay = 0; aay < aadepth; aay++) {
                    aaIndex = aay*aadepth + aax;
                    tempRGB[aaIndex] = (RGBType){0,0,0};
                    // the image is wider than it is tall
                    if(aadepth == 1){
                        xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(float)height)/2);
                        yamnt = ((height - y) + 0.5)/height;
                    }
                    else{
                        xamnt = ((x + aax/(aadepth - 1))/width)*aspectratio - (((width-height)/(float)height)/2);
                        yamnt = ((height - y) + aax/(aadepth - 1))/height;
                    }

                    Vector3 camRayOrigin = RTcam->campos;
                    Vector3 tmp_camDir = RTcam->camDir, tmp_camRight = RTcam->camRight, tmp_camDown = RTcam->camDown;

                    Vector3 camRayDirection = *Vector3_normalize(Vector3_add(&tmp_camDir, Vector3_add(Vector3_multiplie(&tmp_camRight, xamnt - 0.5),
                                                                                                       Vector3_multiplie(&tmp_camDown, yamnt - 0.5))));

                    Ray camRay = newRay(camRayOrigin, camRayDirection);


                    for (int i = 0; i < len; i++) {
                        intersections[i] = Object_findIntersection(RTscene[i], camRay);
                    }

                    int indexOfWinningObject = winningObjectIndex(intersections, len);
                    float winningIntersection = intersections[indexOfWinningObject];

                    if (indexOfWinningObject != -1 && winningIntersection > accuracy) {
                        // determine the position and direction vectors at the point of intersection
                        Vector3 tmp_camRayDirection = camRayDirection;
                        Vector3_add(&camRayOrigin, Vector3_multiplie(&tmp_camRayDirection, winningIntersection));

                        tempRGB[aaIndex] = getColorAt(camRayOrigin, camRayDirection, RTscene, indexOfWinningObject, RTlightes, accuracy, ambientlight, 0, len);
                    }
                }
            }
            // average the pixel color
            Color total = newColor(0.0,0.0,0.0,0.0,0.0);
            for (int i = 0; i < s_aadepth; i++)
                Color_AddRGB(&total, &tempRGB[i]);

            pixels[thisone] = Color_toRGB(Color_Scalar(&total, 1.0/s_aadepth));

            fpixels[thisone] = RGB_brightness(&pixels[thisone]);
        }
    }
    free(tempRGB);
    free(intersections);
}

const char *scale = "$@B8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?+_-~<>i!lI;:,\"^`\'.  ";

void RTshow(){
	int count = strlen(scale)-1;
	char str[width+1];
	str[width] = '\0';
	for (int y = 0; y < height; y+=2) {
		for (int x = 0; x < width; x+=1) {
			thisone = y*width + x;
			float avg = fpixels[thisone];
			char c = scale[count-(int)((float)(count)*avg)];
			str[x] = c;
		}
		str[width] = '\0';
		mvprintw((height-y)/2,0,str);
	}
}

#endif // RENDER_H_INCLUDED
