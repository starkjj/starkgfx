#pragma once
#include "cglm/struct.h"

typedef struct {
    vec3s pos, dir, up;
    float fov, near_clip, far_clip;
    float pitch, yaw;
    float movement_speed, mouse_sensitivity;
    mat4s view, projection;
} camera;

camera* camera_new();
void camera_delete(camera* cam);

vec3s camera_position(camera* cam);
vec3s camera_direction(camera* cam);
mat4s camera_view(camera* cam);
mat4s camera_projection(camera* cam);

void update_camera_vectors(camera* cam);

// Freecam only for now
void camera_movement(camera* cam, vec3s direction, float delta);
void camera_look(camera* cam, float xoffset, float yoffset);
