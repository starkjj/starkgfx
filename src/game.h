#pragma once

#include "cglm/struct.h"

typedef struct game {

    struct {
        vec3s pos, dir, dir_xy, right, up;
        float pitch, yaw;
        float slide;
        float fov;
        mat4s view, proj, view_proj;
        mat4s view_no_pitch, view_proj_no_pitch;
        mat4s int_view, inv_proj, in_view_proj;
    } cam;

    bool quit;

    bool force_mouse_free;

    bool allow_control_input;

    bool allow_editor_picking;

    // add input struct

} game;


// global name
extern game *g;