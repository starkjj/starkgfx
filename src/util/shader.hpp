#pragma once

#include <gl.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath) {
        // Load vertex/frag source from filepath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // Close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESS: " << e.what() << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // Compile shaders
        unsigned int vertex, fragment;

        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        check_compile_error(vertex, "VERTEX");

        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        check_compile_error(fragment, "FRAGMENT");

        // Shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        check_compile_error(ID, "PROGRAM");

        // Delete the shaders (they're linked and no longer needed)
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Activate the shader
    auto use() -> void { glUseProgram(ID); }

    // Utility uniform functions

    auto set_bool(const std::string &name, bool value) const -> void {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    auto set_int(const std::string &name, int value) const -> void {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    auto set_float(const std::string &name, float value) const -> void {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    auto set_vec2(const std::string &name, const glm::vec2 &value) const -> void {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    auto set_vec2(const std::string &name, float x, float y) const -> void {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    auto set_vec3(const std::string &name, const glm::vec3 &value) const -> void {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    auto set_vec3(const std::string &name, float x, float y, float z) const -> void {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    auto set_vec4(const std::string &name, const glm::vec4 &value) const -> void {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    auto set_vec4(const std::string &name, float x, float y, float z, float w) const -> void {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    auto set_mat2(const std::string &name, const glm::mat2 &mat) const -> void {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    auto set_mat3(const std::string &name, const glm::mat3 &mat) const -> void {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    auto set_mat4(const std::string &name, const glm::mat4 &mat) const -> void {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:

    // util class for logging compile errors
    auto check_compile_error(GLuint shader, std::string type) -> void {
        GLint success;
        GLchar info[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, info);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR: " << type << "\n" << info << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, info);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR: " << type << "\n" << info << std::endl;
            }
        }
    }

};