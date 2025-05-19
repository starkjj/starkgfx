#shader vertex
#version 330
uniform mat4 MVP;
in vec3 vCol;
in vec2 vPos;
in vec2 aTexCoord;
out vec3 color;
out vec2 TexCoord;
void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
};

#shader fragment
#version 330
in vec3 color;
in vec2 TexCoord;
out vec4 fragment;
uniform sampler2D texture1;
void main()
{
    fragment = texture(texture1, TexCoord);
};