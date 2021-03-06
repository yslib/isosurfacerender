#version 420 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 frag_normal;
out vec3 frag_pos;

void main()
{
    gl_Position = projection_matrix*view_matrix*model_matrix*vec4(v_pos,1.0);
    frag_pos = vec3(model_matrix*vec4(v_pos,1.0));
    frag_normal = mat3(transpose(inverse(model_matrix)))*v_normal;
}
