SIGNAL_INPUT = 0
TIME_INPUT = 1
FEEDBACK_INPUT = 2
MIX_INPUT = 3
SIGNAL_OUTPUT = 0


setDisplayMode("log")
log(debug.getinfo(1).source)
log("===========================")
log("Input " .. SIGNAL_INPUT .. ": Signal")
log("Input " .. TIME_INPUT .. ": Delay time")
log("Input " .. FEEDBACK_INPUT .. ": Feedback")
log("Input " .. MIX_INPUT .. ": Dry/Wet")
log("Output " .. SIGNAL_OUTPUT .. ": Signal")


delayTime = 0
feedback = 0
lastSampleRate = 0
lastDelayTime = 0


function initBuffer(sampleRate)
    buffer = {}
    for i = 0, sampleRate * 2 - 1 do
        buffer[i] = 0
    end

    i1 = 0
end


function process(sampleRate, sampleTime)
    if isInputConnected(TIME_INPUT) then
        delayTime = clamp(getVoltage(TIME_INPUT), 0.001, 10) / 10
    else
        delayTime = 0.25
    end

    if isInputConnected(FEEDBACK_INPUT) then
        feedback = clamp(getVoltage(FEEDBACK_INPUT), 0, 10) / 10
    else
        feedback = 0.4
    end

    if isInputConnected(MIX_INPUT) then
        mix = clamp(getVoltage(MIX_INPUT), 0, 10) / 10
    else
        mix = 0.3
    end

    if lastSampleRate ~= sampleRate then
        initBuffer(sampleRate)
        lastSampleRate = sampleRate
    end

    if lastDelayTime ~= delayTime then
        length = math.floor(sampleRate * delayTime) * 2
        i1 = 0
        i2 = length / 2
        lastDelayTime = delayTime
    end

    local v = getVoltage(SIGNAL_INPUT)

    buffer[i2] = buffer[i1] * feedback + v

    setVoltage(SIGNAL_OUTPUT, v * (1 - mix) + buffer[i1] * mix)

    i1 = i1 + 1
    if i1 == length then
        i1 = 0
    end

    i2 = i2 + 1
    if i2 == length then
        i2 = 0
    end
end
