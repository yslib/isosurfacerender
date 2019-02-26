#version 420 core
layout(triangles, equal_spacing, cw) in;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
out vec3 frag_normal;
out vec3 frag_pos;
in vec3 frag_normal_from_tcs[];
in vec3 frag_pos_from_tcs[];
struct OutputPatch
{
    vec3 WorldPos_B030;
    vec3 WorldPos_B021;
    vec3 WorldPos_B012;
    vec3 WorldPos_B003;
    vec3 WorldPos_B102;
    vec3 WorldPos_B201;
    vec3 WorldPos_B300;
    vec3 WorldPos_B210;
    vec3 WorldPos_B120;
    vec3 WorldPos_B111;
    vec3 Normal[3];
    vec2 TexCoord[3];
};
in patch OutputPatch oPatch;
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
    
}



vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    vec3 t = vec3(gl_TessCoord.x,gl_TessCoord.y,gl_TessCoord.z);
    return normalize((vec3(2)*t*t - vec3(3)*t-vec3(1)) * v0 + (-vec3(4) * t*t + vec3(4) * t) * v1 + ( vec3(2) * t * t -t )*v2);
}
void main()
{

    frag_normal = interpolate3D(oPatch.Normal[0], oPatch.Normal[1], oPatch.Normal[2]);
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);
    frag_pos = oPatch.WorldPos_B300 * wPow3 +
                    oPatch.WorldPos_B030 * uPow3 +
                    oPatch.WorldPos_B003 * vPow3 +
                    oPatch.WorldPos_B210 * 3.0 * wPow2 * u +
                    oPatch.WorldPos_B120 * 3.0 * w * uPow2 +
                    oPatch.WorldPos_B201 * 3.0 * wPow2 * v +
                    oPatch.WorldPos_B021 * 3.0 * uPow2 * v +
                    oPatch.WorldPos_B102 * 3.0 * w * vPow2 +
                    oPatch.WorldPos_B012 * 3.0 * u * vPow2 +
                    oPatch.WorldPos_B111 * 6.0 * w * u * v;



    // vec4 pos = (gl_TessCoord.x * gl_in[0].gl_Position) +
    // (gl_TessCoord.y * gl_in[1].gl_Position)+
    // (gl_TessCoord.z * gl_in[2].gl_Position);

    gl_Position = projection_matrix*view_matrix*model_matrix*vec4(frag_pos,1.0);

    // for(int i = 0 ; i < 3;i++)
    // {
    //     frag_pos = vec3(model_matrix*vec4(frag_pos_from_tcs[i],1.0));
    //     frag_normal = mat3(transpose(inverse(model_matrix)))*frag_normal_from_tcs[i];
    // }

}