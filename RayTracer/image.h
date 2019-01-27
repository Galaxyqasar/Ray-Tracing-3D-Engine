#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"

typedef struct Image{
    RGBType *data;
    int width, height;
} Image;

void savebmp (const char *filename, int w, int h, int dpi, RGBType *data) {
    FILE *f;
    int k = w*h;
    int s = 4*k;
    int filesize = 54 + s;

    double factor = 39.375;
    int m = (int)factor;

    int ppm = dpi*m;

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

    memcpy(&bmpfileheader[ 2], &filesize,   sizeof(int));
    memcpy(&bmpinfoheader[ 4], &w,          sizeof(int));
    memcpy(&bmpinfoheader[ 8], &h,          sizeof(int));
    memcpy(&bmpinfoheader[21], &s,          sizeof(int));
    memcpy(&bmpinfoheader[25], &ppm,        sizeof(int));
    memcpy(&bmpinfoheader[29], &ppm,        sizeof(int));

    f = fopen(filename,"wb");

    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);

    for (int i = 0; i < k; i++) {
        RGBType rgb = data[i];

        float red = (rgb.r)*255;
        float green = (rgb.g)*255;
        float blue = (rgb.b)*255;

        unsigned char color[3] = {(unsigned char)floor(blue),(unsigned char)floor(green),(unsigned char)floor(red)};

        fwrite(color,1,3,f);
    }

    fclose(f);
}

Image loadImage(char *filename){
    FILE *f = fopen(filename, "rb");
    char bmpfileheader[14];
    char bmpinfoheader[40];
    fread(bmpfileheader, 1, 14, f);
    fread(bmpinfoheader, 1, 40, f);
    int width , height;
    memcpy(&width, &bmpinfoheader[4], sizeof(int));
    memcpy(&height, &bmpinfoheader[8], sizeof(int));
    int n = width * height;
    Image image = {malloc(sizeof(RGBType)*(n+1)), width, height};
    if(image.data != NULL){
        for(int i = 0; i < n; i++){
            float r, g, b;
            unsigned char color[3];
            fread(color,1,3,f);
            b = ((float)color[0])/255.0;
            g = ((float)color[1])/255.0;
            r = ((float)color[2])/255.0;
            RGBType current = (RGBType){r,g,b};
            image.data[i] = current;
        }
    }
    fclose(f);
    return image;
}

void saveImage(Image image, char *filename){
    savebmp(filename, image.width, image.height, 72, image.data);
}

#endif // IMAGE_H_INCLUDED
