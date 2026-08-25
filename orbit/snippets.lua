-- Sections are split by a "--- name" line. One returning a number or a vector
-- becomes a value, one returning a function becomes a callable deck.yaml can
-- sample. Every section sees `t` and every other name here. Saving re-reads it.

--- R
-- param() declares a tunable on first use, shown in the Tuner (key A) and saved
-- with Ctrl+S. The sections below read it by name.
return param("orbit/radius", 1.15, 0.7, 2.0)

--- tilt
-- sinceKeyframe rises to 1 across the transition into the frame marked
-- `keyframe: tilted`, then holds. duringKeyframe spans one slide only.
return 0.6 * t:sinceKeyframe("tilted")

--- moon
-- Three components, so deck.yaml reads this as a point of the 3D scene, for the
-- `point:` object and for the label that follows it.
local a = t.from_begin * param("orbit/speed", 0.6, 0.0, 3.0)
return vec3(R * math.cos(a),
            R * math.sin(a) * math.cos(tilt),
            R * math.sin(a) * math.sin(tilt))

--- ring
-- A `curve:` item samples this over the unit segment, so the section maps [0,1]
-- itself and the item needs no `u:` key.
return function(s)
    local a = s * 2 * math.pi
    return vec3(R * math.cos(a),
                R * math.sin(a) * math.cos(tilt),
                R * math.sin(a) * math.sin(tilt))
end

--- planet
-- The same for a `surface:` item, over the unit square. It takes one vec2
-- argument.
return function(uv)
    local r = 0.55
    local theta, phi = uv.x * 2 * math.pi, uv.y * math.pi
    return vec3(r * math.sin(phi) * math.cos(theta),
                r * math.sin(phi) * math.sin(theta),
                r * math.cos(phi))
end
