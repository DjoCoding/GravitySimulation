#ifndef OBJECTS_H
#define OBJECTS_H

#include "object.h"

typedef struct {
    Object *items;
    size_t count;
    size_t size;
} Objects;

void objects_update(Objects *objs, float dt);
void objects_clean(Objects *objs);
void objects_remove(Objects *objs);



#endif