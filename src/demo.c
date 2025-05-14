//------------------------------------------------------------------------------
//  Simple C99 cimgui+sokol starter project for Win32, Linux and macOS.
//------------------------------------------------------------------------------
#include "../libs/cimgui/cimgui.h"
#include "../libs/sokol/sokol_app.h"
#include "../libs/sokol/sokol_gfx.h"
#include "../libs/sokol/sokol_glue.h"
#include "../libs/sokol/sokol_imgui.h"
#include "../libs/sokol/sokol_log.h"
#include "shader/triangle.h"

static struct {
    sg_pipeline pipline;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });
    simgui_setup(&(simgui_desc_t){ 0 });

    // a vertex buffer with 3 vertices
    float vertices[] = {
        // positions            // colors
        0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices),
        .label = "vertices"
    });

    sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

    state.pipline = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [ATTR_triangle_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_triangle_color0].format = SG_VERTEXFORMAT_FLOAT4,
            }
        },
        .label = "triangle-pipeline"
    });

    // initial clear color
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.0f, 0.5f, 1.0f, 1.0 } }
    };
}

static void frame(void) {
    simgui_new_frame(&(simgui_frame_desc_t){
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });

    /*=== UI CODE STARTS HERE ===*/
    igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once);
    igSetNextWindowSize((ImVec2){400, 100}, ImGuiCond_Once);
    igBegin("Hello Dear ImGui!", 0, ImGuiWindowFlags_None);
    igColorEdit3("Background", &state.pass_action.colors[0].clear_value.r, ImGuiColorEditFlags_None);
    igEnd();
    /*=== UI CODE ENDS HERE ===*/

    sg_begin_pass(&(sg_pass){ .action = state.pass_action, .swapchain = sglue_swapchain() });
    simgui_render();

    sg_apply_pipeline(state.pipline);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);

    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
}

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "StarkGFX",
        .width = 800,
        .height = 600,
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}
