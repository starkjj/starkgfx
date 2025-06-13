#pragma once
#include <gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include "glm/ext/matrix_transform.hpp"

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // euler Angles
    float pitch;
    float yaw;
    // camera options
    float walk_speed;
    float sprint_speed;
    float movement_speed;
    float mouse_sensitivity;
    float fov;

    // constructor with vectors
    Camera() {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        world_up = up;
        yaw = -90.0f;
        pitch = 0.0f;
        walk_speed = 0.001f;
        sprint_speed = 0.002f;
        movement_speed = 0.001f;
        mouse_sensitivity = 0.1f;
        fov = 60.0f;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 get_view_mat() { return glm::lookAt(position, position + front, up); }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera
    // defined ENUM (to abstract it from windowing systems)

    void ProcessKeyboard(float dt) {
        auto key = SDL_GetKeyboardState(NULL);
        float velocity = movement_speed * dt;
        glm::vec2 direction{};

        if (key[SDL_SCANCODE_W]) {
           direction.y = 1.0f;
        }
        if (key[SDL_SCANCODE_S]) {
            direction.y = -1.0f;
        }
        if (key[SDL_SCANCODE_A]) {
            direction.x = -1.0f;
        }
        if (key[SDL_SCANCODE_D]) {
            direction.x = 1.0f;
        }

        movement_speed = key[SDL_SCANCODE_LSHIFT] ? sprint_speed : walk_speed;

        normalize(direction);

        if (direction.y != 0.0f) {
            position += direction.y * front * velocity;
        }

        if (direction.x != 0.0f) {
            position += direction.x * right * velocity;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // update front, right and up Vectors using the updated Euler angles
        updateCameraVectors();
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // calculate the new front vector
        glm::vec3 newfront;
        newfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newfront.y = sin(glm::radians(pitch));
        newfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newfront);
        // also re-calculate the Right and Up vector
        right = glm::normalize(
                glm::cross(front, world_up)); // normalize the vectors, because their length gets closer to 0 the more
                                             // you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
};
