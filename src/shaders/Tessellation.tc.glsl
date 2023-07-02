#version 440 core

struct V2T
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

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


uniform float tessellationLevel;

//< output patch size is the number of vertices in the output patch
layout (vertices = 3) out;

in V2T vdata[];

patch out PnPatch pnPatch;

float vij(int i, int j, vec3 P[3], vec3 N[3])
{
	vec3 Pj_minus_Pi = P[j] - P[i];
	vec3 Ni_plus_Nj  = N[i] + N[j];
	return 2.0*dot(Pj_minus_Pi, Ni_plus_Nj)/dot(Pj_minus_Pi, Pj_minus_Pi);
}

void main()
{
    vec3 P[3];

    pnPatch.b300 = P[0] = vdata[0].position;
    pnPatch.b030 = P[1] = vdata[1].position;
    pnPatch.b003 = P[2] = vdata[2].position;

    vec3 N[3];

    pnPatch.n200 = N[0] = normalize(vdata[0].normal);
    pnPatch.n020 = N[1] = normalize(vdata[1].normal);
    pnPatch.n002 = N[2] = normalize(vdata[2].normal);

    //< calculate control points

    //< tangent control points
    pnPatch.b210 = (2.0*P[0] + P[1] - dot(P[1] - P[0], N[0]) * N[0])/3.0;
    pnPatch.b120 = (2.0*P[1] + P[0] - dot(P[0] - P[1], N[1]) * N[1])/3.0;
    pnPatch.b021 = (2.0*P[1] + P[2] - dot(P[2] - P[1], N[1]) * N[1])/3.0;
    pnPatch.b012 = (2.0*P[2] + P[1] - dot(P[1] - P[2], N[2]) * N[2])/3.0;
    pnPatch.b102 = (2.0*P[2] + P[0] - dot(P[0] - P[2], N[2]) * N[2])/3.0;
    pnPatch.b201 = (2.0*P[0] + P[2] - dot(P[2] - P[0], N[0]) * N[0])/3.0;

    //pnPatch.b210 = (2.0 * P[0] + P[1]) / 3.0;
    //pnPatch.b120 = (2.0 * P[1] + P[0]) / 3.0;
    //pnPatch.b021 = (2.0 * P[1] + P[2]) / 3.0;
    //pnPatch.b012 = (2.0 * P[2] + P[1]) / 3.0;
    //pnPatch.b102 = (2.0 * P[2] + P[0]) / 3.0;
    //pnPatch.b201 = (2.0 * P[0] + P[2]) / 3.0;

	vec3 E = (pnPatch.b210 + pnPatch.b120 + pnPatch.b021 + pnPatch.b012 + pnPatch.b102 + pnPatch.b201)/6.0;
	vec3 V = (P[0] + P[1] + P[2])/3.0;
    pnPatch.b111 = E + (E - V)*0.5;

    //< calculate normal
	pnPatch.n110 = normalize(N[0] + N[1] - vij(0, 1, P, N) * (P[1] - P[0]));
	pnPatch.n011 = normalize(N[1] + N[2] - vij(1, 2, P, N) * (P[2] - P[1]));
	pnPatch.n101 = normalize(N[2] + N[0] - vij(2, 0, P, N) * (P[0] - P[2]));

    //< color
    pnPatch.c100 = vdata[0].color;
	pnPatch.c010 = vdata[1].color;
	pnPatch.c001 = vdata[2].color;


    /*
        - The interpretation of tessellation level depends on the abstract patch type, but the general idea is this:
        - In common cases, each tessellation level defines how many segments an edge is tessellated into( 4 means  one edge ==> 4 edges (2 vertices become 5)).
        - The outer tessellation levels define the tessellation for the outer edges of the patch.
        - The inner tessellation levels are for the number of tessellations within the patch.
        - Triangles only uses 1 inner level and 3 outer levels. The rest are ignored.
        - The patch can be discarded if any outer tessellation level <= 0, it gives chance to TCS to cull patches by passing 0 for a relevant outer tessellation level 
        - The effective tessellation levels = function{ value here, spacing policy }

    */
	gl_TessLevelInner[0] = tessellationLevel;
	gl_TessLevelOuter[0] = tessellationLevel;
	gl_TessLevelOuter[1] = tessellationLevel;
	gl_TessLevelOuter[2] = tessellationLevel;

}