uniform bvec2 b_bvec2;
uniform bvec3 b_bvec3;
uniform bvec4 b_bvec4;

uniform float f_float;
uniform vec2  f_vec2;
uniform vec3  f_vec3;
uniform vec4  f_vec4;

uniform int   i_int;
uniform ivec2 i_ivec2;
uniform ivec3 i_ivec3;
uniform ivec4 i_ivec4;

uniform mat2 m_mat2;
uniform mat3 m_mat3;
uniform mat4 m_mat4;

void main()
{
    bvec4 b = bvec4(b_bvec2, b_bvec2);
    b.x = b.x || bvec4(b_bvec3, true).x;
    b.x = b.x || b_bvec4.x;

    vec4 f = vec4(f_vec2, f_vec2);
    f += vec4(f_vec3, f_float);
    f += f_vec4;

    ivec4 i = ivec4(i_ivec2, i_ivec2);
    i += ivec4(i_ivec3, i_int);
    i += i_ivec4;

    mat2 m2 = m_mat2;
    mat3 m3 = m_mat3;
    mat4 m4 = m_mat4;

    if (f_float > 0.0 || i_int > 0 ||
        b.x || f.x > 0.0 || i.x > 0 ||
        m2[0][0] > 0.0 || m3[0][0] > 0.0 || m4[0][0] > 0.0)
    {
        updateVertex();
    }
}
