#shader vertex
#version 330
uniform mat4 MVP;
in vec3 vCol;
in vec2 vPos;
out vec3 color;
void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
};

#shader fragment
#version 330
in vec3 color;
out vec4 fragment;
void main()
{
    fragment = vec4(color, 1.0);
};