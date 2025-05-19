#pragma once

#include <stdio.h>
#include <string.h>

#define MAX_LINE 1024
#define MAX_SIZE 4096

typedef enum { NONE = -1, VERTEX = 0, FRAGMENT = 1 } shader_type;

typedef struct {
    char *vertex;
    char *fragment;
} shader_data;

static shader_data parse_shader(const char *filepath) {
    shader_type type = NONE;
    char buffer[MAX_SIZE];
    char vertex[MAX_SIZE] = {0};
    char fragment[MAX_SIZE] = {0};

    FILE *file = fopen(filepath, "r");
    if (!file) {
        printf("Error opening file %s\n", filepath);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "#shader vertex")) {
            type = VERTEX;
            continue;
        }

        if (strstr(line, "#shader fragment")) {
            type = FRAGMENT;
            continue;
        }

        if (type == VERTEX) {
            strncat(vertex, line, MAX_LINE);
        } else if (type == FRAGMENT) {
            strncat(fragment, line, MAX_LINE);
        }
    }

    fclose(file);

    return (shader_data) {vertex, fragment};
}
