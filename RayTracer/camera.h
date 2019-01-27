#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct Camera {
    Vector3 campos;
    Vector3 camDir, camRight, camDown;
} Camera;

Camera m_newCamera(Vector3 pos, Vector3 dir, Vector3 right, Vector3 down){
    return (Camera){pos, dir, right, down};
}

Camera newCamera(Vector3 campos, Vector3 lookAt){
    Vector3 diffBtw = newVector3(campos.x - lookAt.x, campos.y - lookAt.y, campos.z - lookAt.z);
    Vector3 camDir = *Vector3_normalize(Vector3_invert(&diffBtw)), tmp_Y = newVector3(0,1,0);
    Vector3 camRight = *Vector3_normalize(Vector3_crossProduct(&tmp_Y, &camDir)), tmp_camRight = camRight;
    Vector3 camDown = *Vector3_crossProduct(&tmp_camRight, &camDir);

    return m_newCamera(campos, camDir, camRight, camDown);
}

#endif // CAMERA_H







