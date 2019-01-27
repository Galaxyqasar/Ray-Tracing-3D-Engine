#ifndef COLOR_H
#define COLOR_H

typedef struct RGBType {
    float r;
    float g;
    float b;
} RGBType;

typedef struct Color{
    float r, g, b, s, a;
} Color;

float RGB_brightness(RGBType *rgb){
    return (rgb->r + rgb->g + rgb->b)/3;
}

Color newColor(float r, float g, float b, float s, float a){
    return (Color){r,g,b,s,a};
}

float Color_brightness(Color *color) {
    return(color->r + color->g + color->b)/3;
}

Color* Color_Scalar(Color *color, float scalar) {
    *color = (Color){color->r*scalar, color->g*scalar, color->b*scalar, color->s, color->a};
    return color;
}
Color* Color_Add(Color *a, Color *b) {
    a->r += b->r;
    a->g += b->g;
    a->b += b->b;
    return a;
}

Color* Color_AddRGB(Color *a, RGBType *b) {
    a->r += b->r;
    a->g += b->g;
    a->b += b->b;
    return a;
}

Color* Color_Multiply(Color *a, Color *b) {
    a->r *= b->r;
    a->g *= b->g;
    a->b *= b->b;
    return a;
}

Color* Color_Average(Color *a, Color *b) {
    a->r = (a->r + b->r)/2;
    a->g = (a->g + b->g)/2;
    a->b = (a->b + b->b)/2;
    return a;
}

Color* Color_Clip(Color *color) {
    float alllight = color->r + color->g + color->b;
    float excesslight = alllight - 3;
    if (excesslight > 0 && alllight != 0) {
        color->r += excesslight*(color->r/alllight);
        color->g += excesslight*(color->g/alllight);
        color->b += excesslight*(color->b/alllight);
    }
    if      (color->r > 1) {color->r = 1;}
    else if (color->r < 0) {color->r = 0;}
    if      (color->g > 1) {color->g = 1;}
    else if (color->g < 0) {color->g = 0;}
    if      (color->b > 1) {color->b = 1;}
    else if (color->b < 0) {color->b = 0;}

    return color;
}

RGBType Color_toRGB(Color *color){
    return (RGBType){color->r, color->g, color->b};
}

Color Color_fromRGB(RGBType rgb){
    return(Color){rgb.r, rgb.g, rgb.b, 0.0, 0.0};
}

#endif // COLOR_H
