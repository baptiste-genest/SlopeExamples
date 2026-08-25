// No #version line, so slope prepends its prelude with iResolution, iTime,
// iHovered, fragColor and the deck's TimeObject.
#include <colormap.glsl>

// fed by the snippet sections of the same name, listed bare in deck.yaml
uniform vec2 src_a;
uniform vec2 src_b;

// declared with a type in deck.yaml, so each one is a slider in the Tuner
uniform float freq;
uniform float falloff;
uniform float amplitude;
uniform vec4  tint;   // declared as `color` in the deck, so it gets a picker

// `view:` in deck.yaml says which part of the plane this draws. iWorld() is the
// point of it under the fragment, and the sources are in the same space.
float ripple(vec2 p, vec2 s)
{
    float d = length(p - s);
    return sin(freq * d - from_begin * 1.6) / (1.0 + falloff * d * d);
}

void main()
{
    vec2 p = iWorld();

    float v = ripple(p, src_a) + ripple(p, src_b);

    // sinceKeyframe takes the deck's name for the frame, rises across the
    // transition into it, then holds. from_action would restart on every step.
    float lines = sinceKeyframe("contours");
    v += lines * 0.30 * sin(16.0 * v);

    vec3 col = plasma(signedRemap(v, amplitude)) * tint.rgb;

    // the deck hangs a label on each of these with `follow:`
    col = mix(col, vec3(1.0), smoothstep(0.05, 0.035, length(p - src_a)));
    col = mix(col, vec3(1.0), smoothstep(0.05, 0.035, length(p - src_b)));

    // iHovered is 1 while the cursor is over the rect
    col += 0.05 * iHovered;

    // the deck fades the primitive itself, so this draws opaque
    fragColor = vec4(col, 1.0);
}
