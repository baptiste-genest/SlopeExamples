-- The two sources. Each returns a vec2, which deck.yaml feeds to the uniform of
-- the same name and also uses to place a label on that point of the shader.

--- src_a
local a = t.from_begin * 2
return vec2(-0.55 + 0.18 * math.cos(a), 0.30 * math.sin(a))

--- src_b
local a = t.from_begin * 2 + math.pi
return vec2(0.55 + 0.18 * math.cos(a), 0.30 * math.sin(a))
