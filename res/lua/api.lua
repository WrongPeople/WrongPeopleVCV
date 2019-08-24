-- Lua module API stub.


-- Display

---
--- Sets the display mode.
---@param mode string | '"disabled"' | '"log"' | '"points"' | '"values"' | '"scope"'
function setDisplayMode(mode) end

---
--- Clear log messages.
--- Works only in `log` display mode.
function clearLog() end

---
--- Add log message.
--- Works only in `log` display mode.
---@param message string
function log(message) end

---
--- Show one of 3 available points.
--- Works only in `points` display mode.
---@param point number
function showPoint(point) end

---
--- Hide one of 3 available points.
--- Works only in `points` display mode.
---@param point number
function hidePoint(point) end

---
--- Set point position and optionally direction (in degree, from -180 to 180).
--- Works only in `points` display mode.
---@param point number
---@param x number
---@param y number
---@param dir number | nil
function setPoint(point, x, y, dir) end

---
--- Show one of 18 (in values mode) or 4 (in scope mode) available values.
--- Works only in `values` and `scope` display modes.
---@param index number
function showValue(index) end

---
--- Hide one of 18 (in values mode) or 4 (in scope mode) available values.
--- Works only in `values` and `scope` display modes.
---@param index number
function hideValue(index) end

---
--- Set one of (in values mode) or 4 (in scope mode) available values.
--- Works only in `values` and `scope` display modes.
---@param index number
---@param value number
function setValue(index, value) end


-- Display, scope mode

---
--- Set value scale in V/div.
--- Works only in `scope` display mode.
---@param scale number
function setScopeScale(scale) end

---
--- Set value offset in volts.
--- Works only in `scope` display mode.
---@param pos number
function setScopePos(pos) end

---
--- Set time resolution in ms/div.
--- Works only in `scope` display mode.
---@param time number
function setScopeTime(time) end

---
--- Set trigger threshold.
--- Works only in `scope` display mode.
---@param value number
function setScopeTrigThreshold(value) end

---
--- Set index of value to use for trigger.
--- Pass -1 for disable.
--- Works only in `scope` display mode.
---@param index number
function setScopeTrigValue(index) end

---
--- Trigger scope.
--- Works only in `scope` display mode.
function scopeTrig() end


-- Inputs

---
--- Returns whether a cable is connected to the input port.
---@param input number
---@return boolean
function isInputConnected(input) end

---
--- Returns the voltage of the given channel.
---@param input number
---@param channel number|nil
---@return number
function getVoltage(input, channel) end

---
--- Returns the given channel's voltage if the port is polyphonic, otherwise returns the first voltage (channel 0).
---@param input number
---@param channel number|nil
---@return number
function getPolyVoltage(input, channel) end

---
--- Returns the voltage if a cable is connected, otherwise returns the given normal voltage.
---@param input number
---@param normalVoltage number
---@param channel number|nil
---@return number
function getNormalVoltage(input, normalVoltage, channel) end

---
--- Returns the voltage if a cable is connected, otherwise returns the given normal voltage.
---@param input number
---@param normalVoltage number
---@param channel number
---@return number
function getNormalPolyVoltage(input, normalVoltage, channel) end

---
--- Returns the sum of all voltages.
---@param input number
---@return number
function getVoltageSum(input) end

---
--- Returns the number of channels.
--- If the port is disconnected, it has 0 channels.
---@param input number
---@return number
function getChannels(input) end

---
--- Returns whether the cable exists and has 1 channel.
---@param input number
---@return boolean
function isMonophonic(input) end

---
--- Returns whether the cable exists and has more than 1 channel.
---@param input number
---@return boolean
function isPolyphonic(input) end


-- Outputs

---
--- Returns whether a cable is connected to the output port.
---@param output number
---@return boolean
function isOutputConnected(output) end

---
--- Sets the voltage of the given channel.
---@param output number
---@param voltage number
---@param channel number|nil
function setVoltage(output, voltage, channel) end

---
--- Sets the number of polyphony channels.
--- Also clears voltages of higher channels.
--- If disconnected, this does nothing (`channels` remains 0).
--- If 0 is given, `channels` is set to 1 but all voltages are cleared.
---@param output number
---@param channels number
function setChannels(output, channels) end
