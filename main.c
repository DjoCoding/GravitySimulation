#include <stdio.h>

#define VEC2_IMPLEMENTATION
#include "./vec2.h"

#include "objects.h"

#define WINDOW_WIDTH  1366
#define WINDOW_HEIGHT 768

#define DELTA_SPEED 3

fV2 camera = {0};
fV2 camera_speed = {0};
float zoom = 1;

float mvpx(float x) {
    return (x - WINDOW_WIDTH / 2 - camera.x) * zoom + WINDOW_WIDTH / 2;
}

float mvpy(float y) {
    return (y - WINDOW_HEIGHT / 2 - camera.y) * zoom + WINDOW_HEIGHT / 2;
}

fV2 mvp(fV2 v) {
    return fv2(mvpx(v.x), mvpy(v.y));
}

void object_render(Object *obj) {
    DrawCircle(mvpx(obj->c.x), mvpy(obj->c.y), obj->radius * zoom, obj->color);

    if (obj->t.count <= 1) { return; }

    for(size_t i = 0; i < obj->t.count - 1; ++i) {
        fV2 p1 = obj->t.items[i];
        fV2 p2 = obj->t.items[i + 1];
        DrawLine(mvpx(p1.x), mvpy(p1.y), mvpx(p2.x), mvpy(p2.y), obj->color);
    }
}

void objects_render(Objects *objs) {
    for (size_t i = 0; i < objs->count; ++i) {
        object_render(&objs->items[i]);
    }
}

Object objects[4] = {
    {
        .c = { 3 * WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2 },            // Initial position within the window
        .speed = { 3 * 5.0f, 0.0f },           // Moving to the right
        .acc = { 0.0f, 0.0f },              // No accelerati on
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
        .speed = { 0.0f, 20.0f },         // Moving diagonally
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

    Objects objs = {0};

    for (size_t i = 0; i < 4; ++i) {
        DA_APPEND(&objs, objects[i]);
    }

    double current = GetTime();

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
            camera = fv2_sub(mvp(objs.items[0].c), fv2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
            zoom = atan(1 * n + PI) + PI / 2;
        }

        if (IsKeyPressed(KEY_S)) {
            n -= 1;
            zoom = tan(1 * n - PI / 2) - PI;
            camera = fv2_sub(mvp(objs.items[0].c), fv2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
        }

        camera = fv2_add(camera, fv2_mul(camera_speed, dt));
        objects_update(&objs, DELTA_SPEED * run * back * dt);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            objects_render(&objs);
        }
        EndDrawing();

        current = now;
    }

    CloseWindow();
    objects_remove(&objs);

    return 0;
}