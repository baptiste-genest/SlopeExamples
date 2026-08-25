-- The shape of the animation, hot-reloaded. main.cpp keeps the grid, the loop
-- and the upload. Sections are split by a "--- name" line.

--- speed
-- param() declares a tunable on first use. It lands in the same Tuner panel as
-- the one main.cpp declares.
return param("ripple/speed", 1.6, 0.0, 5.0)

--- wave
-- Called once per vertex with the squared distance to the centre. t.from_begin
-- is the free running clock, in seconds.
return function(r2)
    return math.sin(8 * r2 - t.from_begin * speed) * math.exp(-1.5 * r2)
end
