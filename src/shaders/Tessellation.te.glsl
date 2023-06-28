#version 440 core

struct PnPatch
{
    vec3 b300;
    vec3 b030;
    vec3 b003;
	vec3 b210;
	vec3 b120;
	vec3 b021;
	vec3 b012;
	vec3 b102;
	vec3 b201;
	vec3 b111;

    vec3 n200;
    vec3 n020;
    vec3 n002;
	vec3 n110;
	vec3 n011;
	vec3 n101;

    vec4 c100;
    vec4 c010;
    vec4 c001;
};

struct T2F
{
    vec3 position;
    vec3 normal;
    vec4 color;
};


layout(triangles, equal_spacing, ccw) in;

patch in PnPatch pnPatch;

out T2F tedata;
out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 lightPosition_worldspace;

void main()
{

    vec3 b300 = pnPatch.b300;
    vec3 b030 = pnPatch.b030;
    vec3 b003 = pnPatch.b003;

    vec3 b210 = pnPatch.b210;
    vec3 b120 = pnPatch.b120;
    vec3 b021 = pnPatch.b021;
    vec3 b012 = pnPatch.b012;
    vec3 b102 = pnPatch.b102;
    vec3 b201 = pnPatch.b201;


    //< The built-in input variable vec3 gl_TessCoord, which comes from tessellator
    //< It is used to identify the coordinate of generated point in "abstrct patch" or "domain"
    //< It is  Barycentric coordinates

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    
    float u3 = u * u * u;
    float v3 = v * v * v;
    float w3 = w * w * w;
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;

    vec3 pos = b300 * w3 + b030 * u3 + b003 * v3
		+ b210 * 3. * w2 * u + b120 * 3. * w * u2 + b201 * 3. * w2 * v
		+ b021 * 3. * u2 * v + b102 * 3. * w * v2 + b012 * 3. * u * v2
		+ b012 * 6. * w * u * v;

    tedata.position = pos;


    vec3 n200 = pnPatch.n200;
    vec3 n020 = pnPatch.n020;
    vec3 n002 = pnPatch.n002;

    vec3 n110 = pnPatch.n110;
    vec3 n011 = pnPatch.n011;
    vec3 n101 = pnPatch.n101;

    tedata.normal = n200 * w2 + n020 * u2 + n002 * v2 + n110 * w * u + n011 * u * v + n101 * w * v;

    vec3 c1 = pnPatch.c100.rgb;
    vec3 c2 = pnPatch.c010.rgb;
    vec3 c3 = pnPatch.c001.rgb;

    tedata.color = vec4(c1 * w + c2 * u + c3 * v, 1.0);

    gl_Position = P * V * M * vec4(pos, 1.0);

    position_worldspace = (M * vec4(pos, 1.0)).xyz;

    vec3 vertexPosition_cameraspace = (V * M * vec4(pos, 1.0)).xyz;
    eyeDirection_cameraspace = vec3(0.0f, 0.0f, 0.0f) - vertexPosition_cameraspace;

    vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

    normal_cameraspace = (V * M * vec4(tedata.normal, 0)).xyz;
}
