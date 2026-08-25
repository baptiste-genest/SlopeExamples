--- pull
-- Read by main.cpp as Snippet::get("pull"). atKeyframe is true on that frame
-- only, so the swing stops when the slide changes. secondsSinceKeyframe counts
-- on the slide's clock.
if t:beforeKeyframe("cost") then return 0.0 end
if t:atKeyframe("cost") then
    return 0.5 - 0.5 * math.cos(t.secondsSinceKeyframe("cost") * 1.1)
end
return 1.0

--- sweeps
-- Also read from main.cpp. Changing the pace here only needs a save.
if t:beforeKeyframe("alternate") then return 6 end
return math.min(10, math.floor(t.secondsSinceKeyframe("alternate") * 0.9))

--- turn
-- star.frag declares turn in deck.yaml, and slope binds it to this section.
return 0.5 - 0.5 * math.cos(t.secondsSinceKeyframe("local_step") * 1.2)

--- fit
-- the same for fit, which stays at 1 while local_step is the current keyframe
return t.duringKeyframe("local_step")
