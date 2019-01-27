#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include "color.h"

typedef struct Light{
    Vector3 position;
    Color color;
} Light;

typedef Light** Lights;

Light newLight (Vector3 p, Color c) {
    return (Light){p, c};
}

#endif // LIGHT_H


