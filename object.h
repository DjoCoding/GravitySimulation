#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include "vec2.h"
#include "da.h"
#include "tracer.h"

typedef struct {
    fV2 c;
    fV2 speed;
    fV2 acc;
    fV2 force;
    size_t mass;
    float radius;
    Color color;
    Tracer t;
} Object;

#define G    6.67 * 1e2


Object object_init(fV2 c, float r, size_t mass, Color color);
fV2 object_get_diff(Object *obj1, Object *obj2);
fV2 object_get_diff_normal(Object *obj1, Object *obj2);
float object_get_factor(Object *obj1, Object *obj2);
fV2 object_get_force(Object *obj1, Object *obj2);
void object_update_acceleration(Object *obj, fV2 a);
void object_update_speed(Object *obj, float dt);
void object_update_position(Object *obj, float dt);
void object_update(Object *obj, float dt);
void object_clean(Object *obj);
void object_remove(Object *obj);


#endif