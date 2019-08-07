-- Just rewritten some of Andrew Belt's classes from Rack SDK.
-- https://github.com/VCVRack/Rack/blob/v1/include/dsp/digital.hpp


SchmittTrigger = {}
SchmittTrigger.__index = SchmittTrigger

function SchmittTrigger.new()
    local self = setmetatable({}, SchmittTrigger)
    self.state = true
    return self
end

function SchmittTrigger:reset()
    self.state = true
end

---
---@param input number
function SchmittTrigger:process(input)
    if self.state then
        if input <= 0 then
            self.state = false
        end
    else
        if input >= 1 then
            self.state = true
            return true
        end
    end
    return false
end

function SchmittTrigger:isHigh()
    return self.state
end


PulseGenerator = {}
PulseGenerator.__index = PulseGenerator

function PulseGenerator.new()
    local self = setmetatable({}, PulseGenerator)
    self.remaining = 0
    return self
end

function PulseGenerator:reset()
    self.remaining = 0
end

---
---@param deltaTime number
function PulseGenerator:process(deltaTime)
    if self.remaining > 0 then
        self.remaining = self.remaining - deltaTime
        return true
    end
    return false
end

---
---@param duration number|nil
function PulseGenerator:trigger(duration)
    duration = duration or 1e-3
    if duration > self.remaining then
        self.remaining = duration
    end
end

