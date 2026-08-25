// The local step, on one cell, in 2D.
//
// turn and fit are declared in deck.yaml and computed in snippets.lua. Editing
// either file changes the panel on the next frame, with no rebuild.
#include <sdf.glsl>

uniform float turn;
uniform float fit;

const int   N     = 7;
const float SHEAR = 0.9;
const float TAU = 6.2831853;

const vec3 PAGE  = vec3(1.000, 1.000, 1.000);
const vec3 CARD  = vec3(0.973, 0.957, 0.929);
const vec3 INK   = vec3(0.129, 0.169, 0.216);
const vec3 REST  = vec3(0.482, 0.545, 0.639);
const vec3 MOVED = vec3(0.886, 0.447, 0.357);
const vec3 FIT   = vec3(0.851, 0.643, 0.255);

float aa;

float line(vec2 p, vec2 a, vec2 b, float w) {
    return smoothstep(w + aa, w - aa, sdSegment2(p, a, b));
}
float disc(vec2 p, vec2 c, float r) {
    return smoothstep(r + aa, r - aa, length(p - c));
}

vec2 restEdge(int k) {
    float a = TAU * float(k) / float(N) + 0.35;
    float r = 0.60 + 0.14 * sin(float(k) * 2.7);
    return r * vec2(cos(a), sin(a));
}

mat2 pushed(float s) {
    return mat2(1.0 + 0.42 * s, 0.78 * s,
               -0.34 * s,       1.0 - 0.18 * s);
}

void main() {
    vec2 p = iUVc();
    aa = 2.0 / iResolution.y;

    mat2 A = pushed(SHEAR);

    float dots = 0.0, crosses = 0.0;
    for (int k = 0; k < N; ++k) {
        vec2 e0 = restEdge(k);
        vec2 e  = A * e0;
        dots    += dot(e0, e);
        crosses += e0.x * e.y - e0.y * e.x;
    }
    float theta = atan(crosses, dots) * turn;
    mat2  R = mat2(cos(theta), sin(theta), -sin(theta), cos(theta));

    vec3 col = PAGE;

    vec2  half_size = vec2(iAspect, 1.0) * 0.92;
    float card = sdRoundBox2(p, half_size, 0.06);
    col = mix(col, vec3(0.0), 0.10 * smoothstep(0.09, -0.02, card));
    col = mix(col, CARD, smoothstep(aa, -aa, card));

    for (int i = 1; i <= 3; ++i) {
        float r = 0.24 * float(i);
        float ring = smoothstep(1.6 * aa, 0.0, abs(length(p) - r));
        col = mix(col, INK, 0.055 * ring);
    }

    for (int k = 0; k < N; ++k) {
        vec2 e0 = restEdge(k);
        col = mix(col, REST, 0.40 * line(p, vec2(0.0), e0, 0.006));
        col = mix(col, REST, 0.55 * disc(p, e0, 0.014));
    }

    for (int k = 0; k < N; ++k) {
        vec2 e0 = restEdge(k);
        vec2 rk = R * e0;
        float glow = exp(-90.0 * sdSegment2(p, vec2(0.0), rk));
        col = mix(col, FIT, fit * (0.75 * line(p, vec2(0.0), rk, 0.007) + 0.18 * glow));
        col = mix(col, FIT, fit * disc(p, rk, 0.016));
    }

    for (int k = 0; k < N; ++k) {
        vec2 e0 = restEdge(k);
        vec2 e  = A * e0;
        col = mix(col, MOVED * 0.85, fit * 0.55 * line(p, R * e0, e, 0.0035));
        float glow = exp(-70.0 * sdSegment2(p, vec2(0.0), e));
        col = mix(col, MOVED, 0.90 * line(p, vec2(0.0), e, 0.009) + 0.16 * glow);
        col = mix(col, MOVED, disc(p, e, 0.019));
    }

    col = mix(col, INK, disc(p, vec2(0.0), 0.026));

    fragColor = vec4(col, 1.0);
}
