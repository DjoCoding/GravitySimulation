#include "object.h"

Object object_init(fV2 c, float r, size_t mass, Color color) {
    Object object = {0};
    object.c = c;
    object.mass = mass;
    object.radius = r;
    object.color = color;
    return object;
}

fV2 object_get_diff(Object *obj1, Object *obj2) {
    return fv2_sub(obj2->c, obj1->c);
}

fV2 object_get_diff_normal(Object *obj1, Object *obj2) {
    return fv2_normalize(object_get_diff(obj1, obj2));
}

float object_get_factor(Object *obj1, Object *obj2) {
    return G * obj1->mass * obj2->mass / pow(fv2_mag(object_get_diff(obj1, obj2)), 2);
}

fV2 object_get_force(Object *obj1, Object *obj2) {
    return fv2_mul(object_get_diff_normal(obj1, obj2), object_get_factor(obj1, obj2));
}

void object_update_acceleration(Object *obj, fV2 a) {
    obj->acc = a;
}

void object_update_speed(Object *obj, float dt) {
    obj->speed = fv2_add(fv2_mul(obj->acc, dt), obj->speed);
}

void object_update_position(Object *obj, float dt) {
    obj->c = fv2_add(fv2_mul(obj->speed, dt), obj->c);
}

void object_update(Object *obj, float dt) {
    DA_APPEND(&obj->t, obj->c);

    object_update_acceleration(obj, fv2_div(obj->force, obj->mass));
    object_update_speed(obj, dt);
    object_update_position(obj, dt);
}

void object_clean(Object *obj) {
    obj->t.count = 0;
}

void object_remove(Object *obj) {
    free(obj->t.items);
}