#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "ray.h"
#include "light.h"
#include "color.h"
#include "vector.h"
#include "camera.h"
#include "3dobjects.h"

int winningObjectIndex(float *object_intersections, int len) {
    // return the index of the winning intersection
    int index_of_minimum_value = -1;
    // prevent unnessary calculations
    if (len == 0)// if there are no intersections
        return -1;
    else if (len == 1) {
        if (object_intersections[0] > 0) // if that intersection is greater than zero then its our index of minimum value
            return 0;
        else     // otherwise the only intersection value is invert
            return -1;
    }
    else {
        // otherwise there is more than one intersection
        // first find the maximum value
        float max = 0.0;
        for (int i = 0; i < len; i++) {
            if (max < object_intersections[i])
                max = object_intersections[i];
        }
        // then starting from the maximum value find the minimum positive value
        if (max <= 0)  // we only want positive intersections
            return -1;
        for (int index = 0; index < len; index++) {
            if (object_intersections[index] > 0 && object_intersections[index] <= max) {
                max = object_intersections[index];
                index_of_minimum_value = index;
            }
        }
        return index_of_minimum_value;
        // all the intersections were invert
    }
}

RGBType getColorAt(Vector3 intersection_position, Vector3 intersecting_ray_direction, Object **scene, int indexOfWinningObject, Light** lightSources, float accuracy, float ambientlight, int n, int len) {
    if(n > 5)
        return (RGBType){0,0,0};
    float *secondary_intersections = malloc(sizeof(float)*len);
    Color winning_objectColor = Object_getColor(scene[indexOfWinningObject], intersection_position);
    float wObjectSpecial = winning_objectColor.s;
    float wObjectAlpha = winning_objectColor.a;

    Vector3 i_intersection_position = *Vector3_invert(&intersection_position);
    Vector3_invert(&intersection_position);
    Vector3 winning_object_normal = Object_getNormalAt(scene[indexOfWinningObject], intersection_position);
    if (wObjectSpecial > 1) {
        // checkered/tile floor pattern
        int square = (int)floor(intersection_position.x) + (int)floor(intersection_position.z);
        if ((square % 2))  // black tile
            winning_objectColor = newColor(0.0, 0.0, 0.0, 0.0, 0.0);
        else                    // white tile
            winning_objectColor = newColor(1.0, 1.0, 1.0, 0.0, 0.0);
    }
    Color finalColor = winning_objectColor;
    Color_Scalar(&finalColor, ambientlight);

    if (wObjectSpecial > 0.0 && wObjectSpecial <= 1.0) {
        // reflection from objects with specular intensity
        Vector3 tmp_intersecting_ray_direction = intersecting_ray_direction;
        float dot1 = Vector3_dotProduct(&winning_object_normal, Vector3_invert(&tmp_intersecting_ray_direction));
        tmp_intersecting_ray_direction = intersecting_ray_direction;
        Vector3 tmp_winning_object_normal = winning_object_normal;
        Vector3 scalar1 = *Vector3_multiplie(&tmp_winning_object_normal, dot1);
        Vector3 add1 = *Vector3_add(&scalar1, &intersecting_ray_direction);
        Vector3 scalar2 = *Vector3_multiplie(&add1, 2);
        Vector3 add2 = *Vector3_add(Vector3_invert(&tmp_intersecting_ray_direction), &scalar2);
        Vector3 reflection_direction = *Vector3_normalize(&add2);
        Ray reflection_ray = newRay(intersection_position, reflection_direction);
        // determine what the ray intersects with first
        for (int i = 0; i < len; i++) {
            secondary_intersections[i] = Object_findIntersection(scene[i], reflection_ray);
        }
        indexOfWinningObject = winningObjectIndex(secondary_intersections, len);
        if (indexOfWinningObject != -1) {
            // reflection ray missed everthing else
            if (secondary_intersections[indexOfWinningObject] > accuracy) {
                // determine the position and direction at the point of intersection with the reflection ray
                // the ray only affects the color if it reflected off something
                Vector3 tmp_reflection_direction = reflection_direction;
                Vector3 tmp_intersection_position = intersection_position;
                Vector3 reflection_intersection_position = *Vector3_add(&tmp_intersection_position,
                                                                        Vector3_multiplie(&tmp_reflection_direction,
                                                                                          secondary_intersections[indexOfWinningObject]));

                Color reflection_intersectionColor = Color_fromRGB(getColorAt(reflection_intersection_position, reflection_direction, scene, indexOfWinningObject, lightSources, accuracy, ambientlight, ++n, len));

                Color_Add(&finalColor, Color_Scalar(&reflection_intersectionColor, wObjectSpecial));
            }
        }
    }
    if(wObjectAlpha > 0.0){
        Color nextColor;
        Vector3 alpha_ray_origin = intersection_position;
        Vector3 alpha_ray_direction = intersecting_ray_direction;
        Ray alpha_ray = newRay(intersection_position, alpha_ray_direction);
        for (int i = 0; i < len; i++) {
            secondary_intersections[i] = Object_findIntersection(scene[i], alpha_ray);
        }
        indexOfWinningObject = winningObjectIndex(secondary_intersections, len);
        if (indexOfWinningObject != -1){
            if (secondary_intersections[indexOfWinningObject] > accuracy) {
                Vector3 tmp_direction = alpha_ray_direction;
                Vector3_add(&intersection_position, Vector3_multiplie(&tmp_direction, secondary_intersections[indexOfWinningObject]));
                nextColor = Color_fromRGB(getColorAt(intersection_position, alpha_ray_direction, scene, indexOfWinningObject, lightSources, accuracy, ambientlight, ++n, len));
                Color_Add(Color_Scalar(&finalColor, 1.0 - wObjectAlpha), Color_Scalar(&nextColor, wObjectAlpha));
            }
        }
    }
    for (int light_index = 0; lightSources[light_index] != NULL; light_index++) {
        Color lightColor = lightSources[light_index]->color;
        Vector3 light_position = lightSources[light_index]->position;
        Vector3 light_direction = *Vector3_normalize(Vector3_add(&light_position, &i_intersection_position));
        float cosine_angle = Vector3_dotProduct(&winning_object_normal, &light_direction);
        if (cosine_angle > 0) {
            // test for shadows
            bool shadowed = false;
            Ray shadow_ray = newRay(intersection_position, light_direction);
            for (int i = 0; i < len; i++) {
                secondary_intersections[i] = Object_findIntersection(scene[i], shadow_ray);
            }
            for (int c = 0; c < len; c++) {
                if (secondary_intersections[c] > accuracy) {
                    if (secondary_intersections[c] <= Vector3_magnitude(&light_direction)) {
                        shadowed = true;
                        break;
                    }
                }
            }
            if (!shadowed) {
                Color_Add(&finalColor, Color_Scalar(Color_Multiply(&winning_objectColor, &lightColor), cosine_angle));
                if (wObjectSpecial > 0.0 && wObjectSpecial <= 1.0) {
                    // special [0-1]
                    Vector3 tmp_intersecting_ray_direction = intersecting_ray_direction;
                    float dot1 = Vector3_dotProduct(&winning_object_normal, Vector3_invert(&tmp_intersecting_ray_direction));
                    tmp_intersecting_ray_direction = intersecting_ray_direction;
                    Vector3 scalar1 = *Vector3_multiplie(&winning_object_normal, dot1);
                    Vector3 add1 = *Vector3_add(&scalar1, &intersecting_ray_direction);
                    Vector3 scalar2 = *Vector3_multiplie(&add1, 2.0);
                    Vector3 add2 = *Vector3_add(Vector3_invert(&tmp_intersecting_ray_direction), &scalar2);
                    //Vector3 reflection_direction = *Vector3_normalize(&add2);

                    float specular = Vector3_dotProduct(Vector3_normalize(&add2), &light_direction);
                    if (specular > 0.0) {
                        specular = pow(specular, 10.0);
                        Color_Add(&finalColor, Color_Scalar(&lightColor, specular*wObjectSpecial));
                    }
                }
            }
        }
    }
    free(secondary_intersections);
    return Color_toRGB(Color_Clip(&finalColor));
}

#endif // FUNCTIONS_H
