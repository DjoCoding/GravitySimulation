#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define VEC2_IMPLEMENTATION
#include "vec2.h"

#define WINDOW_WIDTH  1366
#define WINDOW_HEIGHT 768

#define FORCES_COUNT  10

#define SUN_COLOR    YELLOW
#define EARTH_COLOR  BLUE

#define SUN_MASS    1000
#define EARTH_MASS  1

#define G    6.67 * 1e2

#define DELTA_SPEED 3

#define DA_INITIAL_SIZE  20000

#define DA_FULL(da) ((da)->count >= (da)->size)

#define DA_RESIZE(da) \
    do { \
        if ((da)->size == 0) { (da)->size = DA_INITIAL_SIZE / 2; } \
        (da)->size *= 2; \
        (da)->items = realloc((da)->items, (da)->size * sizeof((da)->items[0])); \
        if (!((da)->items)) { fprintf(stderr, "could not reallocate"); exit(EXIT_FAILURE); } \
    } while(0)

#define DA_APPEND(da, v) \
    do {\
        if (DA_FULL(da)) { DA_RESIZE(da); } \
        (da)->items[(da)->count++] = v; \
    } while(0)

typedef struct {
    fV2 *items;
    size_t count;
    size_t size;
} Tracer;

fV2 camera = {0};
fV2 camera_speed = {0};
float zoom = 1;

typedef struct {
    fV2 *items;
    size_t count;
    size_t size;
} Forces;

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

void object_render(Object *obj) {
    DrawCircle((obj->c.x - camera.x) * zoom, (obj->c.y - camera.y) * zoom, obj->radius * zoom, obj->color);

    if (obj->t.count <= 1) { return; }

    for(size_t i = 0; i < obj->t.count - 1; ++i) {
        fV2 p1 = obj->t.items[i];
        fV2 p2 = obj->t.items[i + 1];
        DrawLine(p1.x * zoom - camera.x, p1.y * zoom - camera.y, p2.x * zoom - camera.x, p2.y * zoom - camera.y, obj->color);
    }
}

void object_clean(Object *obj) {
    obj->t.count = 0;
}

void object_remove(Object *obj) {
    free(obj->t.items);
}

Object sun = {0};
Object earth = {0};

typedef struct {
    Object *items;
    size_t count;
    size_t size;
} Objects;

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

void objects_render(Objects *objs) {
    for (size_t i = 0; i < objs->count; ++i) {
        object_render(&objs->items[i]);
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

Object objects[4] = {
    {
        .c = { 3 * WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2 },            // Initial position within the window
        .speed = { 3 * 5.0f, 0.0f },           // Moving to the right
        .acc = { 0.0f, 0.0f },              // No acceleration
        .mass = 500,                         // Arbitrary mass
        .radius = 35.0f,                     // Radius of the object
        .color = YELLOW,                       // Color: RED
        .t = (Tracer) {0}            // Tracer initialization (assumed)
    },
    {
        .c = { 4 * WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2 },            // Initial position within the window
        .speed = { 0.0f, -20.0f },          // Moving upwards
        .acc = { 0.0f, 0.0f },              // Accelerating downwards (gravity-like)
        .mass = 1,                         // Larger mass
        .radius = 10.0f,                     // Larger radius
        .color = BLUE,                      // Color: BLUE
        .t = (Tracer) {0}            // Tracer initialization (assumed)
    },
    {
        .c = { 7* WINDOW_WIDTH / 10, WINDOW_HEIGHT / 2 },            // Initial position within the window
        .speed = { -20.0f, 20.0f },         // Moving diagonally
        .acc = { 0.0f, 0.0f },              // No acceleration
        .mass = 1,                          // Smaller mass
        .radius = 15.0f,                     // Medium radius
        .color = GREEN,                     // Color: GREEN
        .t = (Tracer) {0}            // Tracer initialization (assumed)
    },
    {
        .c = { 2 * WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2 },            // Initial position within the window
        .speed = { 0.0f, 20.0f },            // Stationary
        .acc = { 0.0f, 0.0f },              // No acceleration
        .mass = 1,                         // Heaviest object
        .radius = 20.0f,                    // Largest radius
        .color = RED,                    // Color: YELLOW
        .t = (Tracer) {0}            // Tracer initialization (assumed)
    }
};


int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gravity simulation");

    sun = object_init(fv2(WINDOW_WIDTH / 3, WINDOW_HEIGHT / 2), 30.0f, SUN_MASS, SUN_COLOR);
    earth = object_init(fv2(9 * WINDOW_WIDTH / 10, WINDOW_HEIGHT / 2), 10.0f, EARTH_MASS, EARTH_COLOR);
    earth.speed.y = 30.0f;

    Objects objs = {0};

    for (size_t i = 0; i < 4; ++i) {
        DA_APPEND(&objs, objects[i]);
    }

    double current = GetTime();

    int key = 0;

    int run = 1;
    int back = 1;

    int n = 0;

    while(!WindowShouldClose()) {
        double now = GetTime();
        double dt = now - current;

        if (IsKeyDown(KEY_G)) {
            run ^= 1;
            objects_clean(&objs);
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            back *= -1;
            objects_clean(&objs);
        }

        if (IsKeyDown(KEY_RIGHT)) {
            camera_speed.x = 700.0f;
        }

        if (IsKeyDown(KEY_LEFT)) {
            camera_speed.x = -700.0f;
        }

        if (IsKeyDown(KEY_UP)) {
            camera_speed.y = -700.0f;
        }

        if (IsKeyDown(KEY_DOWN)) {
            camera_speed.y = 700.0f;
        }

        if (IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT)) {
            camera_speed.x = 0.0f;
        }

        if (IsKeyReleased(KEY_UP) || IsKeyReleased(KEY_DOWN)) {
            camera_speed.y = 0.0f;
        }

        if (IsKeyPressed(KEY_W)) {
            n += 1;
            zoom = atan(1 * n + PI) + PI / 2;
        }

        if (IsKeyPressed(KEY_S)) {
            n -= 1;
            zoom = tan(1 * n - PI / 2) - PI;
        }

        camera = fv2_add(camera, fv2_mul(camera_speed, dt));
        objects_update(&objs, DELTA_SPEED * run * back * dt);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            objects_render(&objs);
            // fV2 center = fv2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
            // fV2 sun = fv2_mul(fv2_sub(objs.items[0].c, camera), zoom);
            // DrawLine(sun.x, sun.y, center.x, center.y, RAYWHITE);
        }
        EndDrawing();

        current = now;
    }

    CloseWindow();
    printf("%zu\n", objects[0].t.size);
    objects_remove(&objs);

    return 0;
}