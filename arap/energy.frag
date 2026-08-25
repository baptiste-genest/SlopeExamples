// The energy after each sweep.
//
// main.cpp writes E_k/E_0 into the array uniform below every frame, so this
// plot shows what the solver just measured.
#include <plot2d.glsl>
#include <sdf.glsl>

uniform float energies[16];    // written by Shader::set(name, vector)
uniform int   energies_count;  // how many values it holds, set by slope
uniform float max_sweeps;

const float DECADES = 3.0;
const float LN10    = 2.302585;

const vec3 PAGE  = vec3(1.000, 1.000, 1.000);
const vec3 INK   = vec3(0.165, 0.200, 0.251);   // the colours commands.tex defines
const vec3 LINE  = vec3(0.353, 0.420, 0.522);
const vec3 NOW   = vec3(0.886, 0.447, 0.357);

float Y = 4.0;
float sweeps = 0.0;

float logE(float k) {
    float e = energies[clamp(int(k), 0, max(energies_count - 1, 0))];
    return clamp(1.0 + log(max(e, 1e-30)) / (LN10 * DECADES), 0.0, 1.0);
}

float gcurve(float x) {
    float xc = clamp(x, 0.0, sweeps);
    float k0 = floor(xc);
    return Y * mix(logE(k0), logE(min(k0 + 1.0, sweeps)), xc - k0);
}

void main() {
    sweeps = float(max(energies_count - 1, 0));

    float xmin = -1.0, xmax = max_sweeps + 1.0;
    float upp  = unitsPerPixel(xmin, xmax);
    float span = iResolution.y * upp;
    Y = 0.66 * span;

    vec2 p = plotPointAt(xmin, xmax, span * 0.30);

    vec3 col = PAGE;

    vec2  centre = vec2(0.5 * (xmin + xmax), span * 0.30);
    float card = sdRoundBox2(p - centre, vec2(iAspect, 1.0) * 0.94 * span * 0.5, 0.3);
    float inside = smoothstep(upp, -upp, card);

    for (int d = 1; d <= int(DECADES); ++d) {
        float y = Y * (1.0 - float(d) / DECADES);
        col = mix(col, INK, 0.07 * inside * stroke(abs(p.y - y), 0.0, 1.2 * upp));
    }
    for (int k = 1; k <= 16; ++k) {
        if (float(k) > max_sweeps) break;
        col = mix(col, INK, 0.05 * inside * stroke(abs(p.x - float(k)), 0.0, 1.2 * upp));
    }
    col = mix(col, INK, 0.40 * inside * axesMask(p, upp));
    col = mix(col, INK, 0.35 * inside * xTickMask(p, 1.0, 6.0, upp));

    float drawn = step(p.x, sweeps + 0.02) * step(-0.02, p.x) * inside;
    float g  = gcurve(p.x);
    float dg = PLOT_SLOPE(gcurve, p.x, upp);

    float area = underCurve(p, g, upp) * step(0.0, p.y) * drawn;
    col = mix(col, LINE, 0.055 * area * (1.0 - 0.75 * p.y / max(g, 1e-3)));

    float glow = exp(-2.5 * graphDist(p.y, g, dg));
    col = mix(col, LINE, drawn * (0.16 * glow + curveMask(p, g, dg, 3.4, upp)));

    for (int k = 0; k <= 16; ++k) {
        if (float(k) > sweeps) break;
        vec2  c    = vec2(float(k), gcurve(float(k)));
        bool  last = float(k) > sweeps - 0.5;
        vec3  tint = last ? NOW : LINE;
        float r    = last ? 6.5 + 1.1 * sin(3.0 * iTime) : 4.4;
        col = mix(col, tint, inside * pointMask(p, c, r + 2.2, upp) * 0.30);
        col = mix(col, tint, inside * pointMask(p, c, r, upp));
        if (!last)
            col = mix(col, PAGE, inside * pointMask(p, c, r - 2.3, upp));
    }

    fragColor = vec4(col, 1.0);
}
