#include "objects.h"


void objects_update(Objects *objs, float dt) {
    for (size_t i = 0; i < objs->count; ++i) {
        fV2 force = fv2v(0);

        for (size_t j = 0; j < objs->count; ++j) {
            if (i == j) { continue; }
            force = fv2_add(force, object_get_force(&objs->items[i], &objs->items[j]));
        }

        objs->items[i].force = force;
        object_update(&objs->items[i], dt);
    }
}

void objects_clean(Objects *objs) {
    for(size_t i = 0; i < objs->count; ++i) {
        object_clean(&objs->items[i]);
    }
}

void objects_remove(Objects *objs) {
    for(size_t i = 0; i < objs->count; ++i) {
        object_remove(&objs->items[i]);
    }
}