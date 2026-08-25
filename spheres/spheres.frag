// Any number of spheres over a floor, rearranged at every step of the deck.
// main.cpp is four lines: the scene, the motion and the shading are all here.
// Every uniform below is declared in deck.yaml, which makes it a knob in the
// Tuner (key A).

#include <camera.glsl>

uniform int   count;       // how many spheres
uniform float radius;      // their radius, before the per-index jitter
uniform float spread;      // radius of the shell they sit on
uniform float floor_z;     // height of the ground
uniform float wobble;      // how far each one strays from its place
uniform float speed;       // how fast it strays
uniform float shade;       // how dark a shadow is, 1 being black
uniform vec3  sun;

uniform vec4 c_warm;
uniform vec4 c_cool;
uniform vec4 c_floor;
uniform vec4 c_sky;

vec2 r2(int i) {
    const float a1 = 0.7548776662466927;
    const float a2 = 0.5698402909980532;
    return fract(0.5 + vec2(a1, a2) * float(i));
}

vec3 hash3(int i) {
    vec3 p = fract(float(i) * vec3(0.1031, 0.1030, 0.0973));
    p += dot(p, p.yxz + 33.33);
    return fract((p.xxy + p.yzz) * p.zyx);
}

float radiusOf(int i) { return radius * (0.65 + 0.7 * hash3(i).x); }

vec3 shellPoint(int i) {
    vec2 q = r2(i);
    float z = 2.0 * q.x - 1.0;
    float a = 6.2831853 * q.y;
    float s = sqrt(max(1.0 - z * z, 0.0));
    return spread * vec3(s * cos(a), s * sin(a), z * 0.6);
}

vec3 poseCloud(int i, float t) {
    vec3 h = hash3(i);
    vec3 w = vec3(sin(t * (0.7 + h.x) + 6.2831853 * h.y),
                  sin(t * (0.9 + h.y) + 6.2831853 * h.z),
                  sin(t * (1.1 + h.z) + 6.2831853 * h.x));
    return shellPoint(i) + wobble * w;
}

vec3 poseRing(int i, float t) {
    float a = 6.2831853 * (float(i) + 0.5) / float(count) + t * 0.35;
    float R = spread * 1.25;
    return vec3(R * cos(a), R * sin(a), 0.18 * sin(3.0 * a + t * 0.8));
}

vec3 poseRain(int i, float t) {
    vec3  h  = hash3(i);
    vec3  xy = shellPoint(i);
    float ph = t * (0.9 + 0.6 * h.y) + 6.2831853 * h.z;
    return vec3(xy.xy, floor_z + radiusOf(i) + spread * 1.5 * abs(sin(ph)));
}

vec3 poseHelix(int i, float t) {
    float u = (float(i) + 0.5) / float(count);
    float a = 12.566371 * u + t * 0.5;
    float R = spread * 0.75;
    return vec3(R * cos(a), R * sin(a), mix(floor_z + 0.30, floor_z + 2.00, u));
}

vec4 sphereAt(int i) {
    float t = iTime * speed;

    // the weight of each of the deck's keyframes. slope replaces the name with
    // its slide index when it compiles the shader, so nothing is bound here.
    float w_cloud = duringKeyframe("cloud");
    float w_ring  = duringKeyframe("ring");
    float w_rain  = duringKeyframe("rain");
    float w_helix = duringKeyframe("helix");

    // two neighbouring weights sum to 1, so this blends the two arrangements
    // the deck is between. Before the first keyframe every weight is 0.
    float sum = w_cloud + w_ring + w_rain + w_helix;
    vec3 c;
    if (sum < 1e-4)
        c = poseCloud(i, t);
    else
        c = (w_cloud * poseCloud(i, t) + w_ring  * poseRing(i, t)
           + w_rain  * poseRain(i, t)  + w_helix * poseHelix(i, t)) / sum;

    return vec4(c, radiusOf(i));
}

float hitSphere(vec3 ro, vec3 rd, vec3 c, float r) {
    vec3  oc   = ro - c;
    float b    = dot(oc, rd);
    float k    = dot(oc, oc) - r * r;
    float disc = b * b - k;
    if (disc < 0.0) return -1.0;
    float sq = sqrt(disc);
    float t  = -b - sq;
    if (t < 1e-4) t = -b + sq;
    return t < 1e-4 ? -1.0 : t;
}

float trace(vec3 ro, vec3 rd, out vec3 nor, out int id) {
    float best = 1e30;
    nor = vec3(0.0, 0.0, 1.0);
    id  = -2;

    for (int i = 0; i < count; ++i) {
        vec4  s = sphereAt(i);
        float t = hitSphere(ro, rd, s.xyz, s.w);
        if (t > 0.0 && t < best) {
            best = t;
            id   = i;
            nor  = (ro + t * rd - s.xyz) / s.w;
        }
    }

    if (rd.z < -1e-6) {
        float t = (floor_z - ro.z) / rd.z;
        if (t > 1e-4 && t < best) {
            best = t;
            id   = -1;
            nor  = vec3(0.0, 0.0, 1.0);
        }
    }
    return best;
}

bool occluded(vec3 p, vec3 l) {
    for (int i = 0; i < count; ++i) {
        vec4 s = sphereAt(i);
        if (hitSphere(p, l, s.xyz, s.w) > 0.0) return true;
    }
    return false;
}

float checkerAt(vec2 p, float scale) {
    vec2 c = floor(p / scale);
    return mod(c.x + c.y, 2.0);
}

vec3 skyColor(vec3 rd) {
    return mix(c_sky.rgb * 0.82, c_sky.rgb, clamp(0.5 + 0.9 * rd.z, 0.0, 1.0));
}

void main() {
    // the ray polyscope would trace through this fragment, so the usual camera
    // controls navigate the scene and a saved view frames it
    vec3 ro, rd;
    polyscopeRay(ro, rd);

    vec3 sky = skyColor(rd);

    vec3 nor;
    int  id;
    float t = trace(ro, rd, nor, id);

    if (id == -2) {
        fragColor = vec4(pow(sky, vec3(0.4545)), 1.0);
        return;
    }

    vec3  p = ro + t * rd;
    vec3  l = normalize(sun);
    float lit = occluded(p + nor * 1e-3, l) ? 1.0 - shade : 1.0;

    float dif = clamp(dot(nor, l), 0.0, 1.0);
    float amb = 0.5 + 0.5 * nor.z;

    float tiles = (1.0 - smoothstep(22.0, 60.0, t)) * checkerAt(p.xy, 0.5);
    vec3 albedo = (id < 0)
        ? c_floor.rgb * mix(0.78, 1.0, tiles)
        : mix(c_cool.rgb, c_warm.rgb, r2(id).y);

    vec3 col = albedo * (dif * lit * vec3(1.05, 0.98, 0.88) * 0.95
                       + amb * c_sky.rgb * 0.35);

    if (id >= 0) {
        vec3 hv = normalize(l - rd);
        col += vec3(1.0) * pow(clamp(dot(nor, hv), 0.0, 1.0), 48.0) * 0.35 * lit;
        col += pow(clamp(1.0 + dot(rd, nor), 0.0, 1.0), 3.0) * 0.18;
    }

    fragColor = vec4(pow(col, vec3(0.4545)), 1.0);
}
