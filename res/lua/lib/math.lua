-- Just rewritten some of Andrew Belt's functions from Rack SDK.
-- https://github.com/VCVRack/Rack/blob/v1/include/math.hpp


--- Limits `x` between `a` and `b`.
--- If `b < a`, returns a.
function clamp(x, a, b)
    math.max(math.min(x, b), a)
end


--- Limits `x` between `a` and `b`.
--- If `b < a`, switches the two values.
function clampSafe(x, a, b)
    if a <= b then
        return clamp(x, a, b)
    else
        return clamp(x, b, a)
    end
end


function rescale(x, xMin, xMax, yMin, yMax)
    return yMin + (x - xMin) / (xMax - xMin) * (yMax - yMin)
end
