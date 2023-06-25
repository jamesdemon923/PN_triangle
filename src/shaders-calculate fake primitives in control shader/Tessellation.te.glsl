#version 440 core

struct TC2E
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct TC2EF
{
    vec3 controlPoints[6];
    vec3 controlNormals[3];
};

struct T2F
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

//< specify domain, spacing policy for Tessellator
/*
    ## input layout qualifiers
    - specify options that control the particular form of tessellation
    - equal_spacing            : clamp to [1, max], rounded up to the nearest integer
    - fractional_even_spacing  : clamp to [2, max], rounded up to the nearest even integer
    - fractional_odd_spacing   : clamp to [1, max - 1], rounded up to the nearest odd integer

    - If all of the effective outer levels (as computed above) levels are exactly 1.0, and the effective inner level is also 1.0,
      then nothing is tessellated, and the TES will get 3 vertices and one triangle. 
*/
layout(triangles, equal_spacing, ccw) in;

//<per-vertex inputs from the TCS are arrays indexed by index implicitly
in TC2E tcdata[];
in TC2EF tcfdata[];
//< per-patch outputs from the TCS as inputs in the TES using the patch keyword
//patch in vec4 data;

out T2F tedata;
out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 lightPosition_worldspace;

//<
//< evaluate surface for given UV coordinate
//< interpolate attributes of vertex 
//< maybe displacement here
//<
/*
    - reference : https://www.khronos.org/opengl/wiki/Tessellation_Evaluation_Shader
    - The inputs for the TES are : per-vertex & per-patch 
    - built-in variables
      - in vec3 gl_TessCoord      : Barycentric coordinates of generated primitive
      - in int gl_PatchVerticesIn : the vertex count for the patch being processed
      - in int gl_PrimitiveID     : the index of the current patch in the series of patches being processed for this draw call

      - buit-in input 
          in gl_PerVertex
            {
              vec4 gl_Position;
              float gl_PointSize;
              float gl_ClipDistance[];
            } gl_in[gl_MaxPatchVertices];

      - buit-in output
          out gl_PerVertex
          {
          vec4 gl_Position;   //< the clip-space output position of the current vertex.
          float gl_PointSize; //< the pixel width/height of the point being rasterized. valid for point primitives
          float gl_ClipDistance[]; //< allows shader to set the distance from the vertex to each User-Defined Clip Plane
          };
*/
void main()
{
    vec3 c1 = tcdata[0].color.rgb;
    vec3 c2 = tcdata[1].color.rgb;
    vec3 c3 = tcdata[2].color.rgb;


    vec3 b300 = tcdata[0].position;
    vec3 b030 = tcdata[1].position;
    vec3 b003 = tcdata[2].position;

    vec3 b210 = tcfdata[0].controlPoints[0];
    vec3 b120 = tcfdata[0].controlPoints[1];
    vec3 b021 = tcfdata[0].controlPoints[2];
    vec3 b012 = tcfdata[0].controlPoints[3];
    vec3 b102 = tcfdata[0].controlPoints[4];
    vec3 b201 = tcfdata[0].controlPoints[5];

    //<
    //< The built-in input variable vec3 gl_TessCoord, which comes from tessellator
    //< It is used to identify the coordinate of generated point in "abstrct patch" or "domain"
    //< It is  Barycentric coordinates
    //<
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

#if 1
    vec3 n200 = tcdata[0].normal;
    vec3 n020 = tcdata[1].normal;
    vec3 n002 = tcdata[2].normal;

    vec3 n110 = tcfdata[1].controlNormals[0];
    vec3 n011 = tcfdata[1].controlNormals[1];
    vec3 n101 = tcfdata[1].controlNormals[2];

    tedata.normal = n200 * w2 + n020 * u2 + n002 * v2
		    + n110 * w * u + n011 * u * v + n101 * w * v;
#else
    tedata.normal = n1 * w + n2 * u + n3 * v;
#endif

    tedata.color = vec4(c1 * w + c2 * u + c3 * v, 1.0);

    gl_Position = P * V * M * vec4(pos, 1.0);

    position_worldspace = (M * vec4(pos, 1.0)).xyz;

    vec3 vertexPosition_cameraspace = (V * M * vec4(pos, 1.0)).xyz;
    eyeDirection_cameraspace = vec3(0.0f, 0.0f, 0.0f) - vertexPosition_cameraspace;

    vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

    normal_cameraspace = (V * M * vec4(tedata.normal, 0)).xyz;
}
