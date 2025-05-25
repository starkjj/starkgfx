#include "camera.h"

camera * camera_new() {
    camera* c = malloc(sizeof(camera));

    c->pos = (vec3s) {0.f, 0.f, 0.f};
    c->up = (vec3s) {0.f, 1.f, 0.f};
    c->dir = (vec3s) {0.f, 0.f, 0.f};
    // c->fov = make a call to graphics header to get window ratio
    c->near_clip = 0.1f;
    c->far_clip = 100.f;
    c->movement_speed = 2.5f;
    c->mouse_sensitivity = 0.1f;

    update_camera_vectors(c);

    return c;
}

void camera_delete(camera* c) {
    free(c);
}

mat4s camera_view(camera* c) {
    vec3s center = glms_vec3_add(c->pos, c->dir);
    return glms_lookat(c->pos, center, c->up);
}

void update_camera_vectors(camera* c) {
    vec3s front;
    front.x = cos(glm_rad(c->yaw)) * cos(glm_rad(c->pitch));
    front.y = sin(glm_rad(c->pitch));
    front.z = sin(glm_rad(c->yaw)) * cos(glm_rad(c->pitch));
    c->dir = glms_normalize(front);
}