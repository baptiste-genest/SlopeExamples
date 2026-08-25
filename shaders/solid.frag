// raymarch.glsl calls sceneSDF, so a shader including it must define one.
//
// The ray comes from polyscope, so this sits in the 3D scene. The mouse orbits
// it and a saved camera view frames it. Polyscope's world is z up.
#include <sdf.glsl>
#include <camera.glsl>
#include <raymarch.glsl>

uniform float blend;
uniform vec4  albedo;

float sceneSDF(vec3 p)
{
    float body = opSmoothUnion(sdSphere(p - vec3(-0.34, 0.0, 0.05), 0.32),
                               sdTorus(vec3(p.x - 0.30, p.z, p.y), vec2(0.34, 0.11)),
                               blend);
    return opUnion(body, sdPlane(p, vec3(0.0, 0.0, 1.0), 0.42));
}

void main()
{
    vec3 ro, rd;
    polyscopeRay(ro, rd);

    vec3 pos;
    // alpha 0 where nothing was hit, so the 3D scene shows through
    if (!marchScene(ro, rd, pos)) {
        fragColor = vec4(0.0);
        return;
    }

    vec3 n = sceneNormal(pos);
    vec3 base = albedo.rgb;
    if (pos.z < -0.41)
        base = mix(vec3(0.78), vec3(0.90), checker(pos.xy, 0.35));

    fragColor = vec4(shadeDefault(pos, n, rd, base, vec3(0.4, -0.6, 0.9)), 1.0);
}
