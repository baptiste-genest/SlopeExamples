// Angle brackets find slope's shader stdlib. The uniforms are in deck.yaml.
#include <sdf.glsl>
#include <colormap.glsl>

uniform float blend;
uniform float bands;
uniform vec3  phase;

float scene(vec2 p)
{
    float d = opSmoothUnion(sdCircle(p - vec2(-0.30, 0.12), 0.26),
                            sdBox2(p - vec2(0.30, 0.12), vec2(0.22)), blend);
    return opSmoothUnion(d, sdNgon(p - vec2(0.0, -0.34), 0.21, 3), blend);
}

void main()
{
    // iWorld() is the point of the `view:` region under this fragment
    vec2 p = iWorld();
    float d = scene(p);

    // cosinePalette tunes a whole scheme from four vec3s. `phase` is a uniform,
    // so the Tuner drives it.
    vec3 col = cosinePalette(signedRemap(d, 0.6),
                             vec3(0.5), vec3(0.5), vec3(1.0), phase);
    col *= 0.88 + 0.12 * cos(bands * d);
    col = mix(col, vec3(1.0), 1.0 - smoothstep(0.0, 0.008, abs(d)));

    fragColor = vec4(col, 1.0);
}
