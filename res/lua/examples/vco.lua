CV_INPUT = 0
SIN_OUTPUT = 0
TRI_OUTPUT = 1
SAW_OUTPUT = 2
SQR_OUTPUT = 3


setDisplayMode("log")
log(debug.getinfo(1).source)
log("===========================")
log("Input " .. CV_INPUT .. ": CV")
log("Output " .. SIN_OUTPUT .. ": Sine")
log("Output " .. TRI_OUTPUT .. ": Triangle")
log("Output " .. SAW_OUTPUT .. ": Saw")
log("Output " .. SQR_OUTPUT .. ": Square")


FREQ_C4 = 261.6256

phase = 0


function process(sampleRate, sampleTime)
    freq = FREQ_C4 * (2 ^ getVoltage(CV_INPUT))

    phase = phase + freq * sampleTime
    phase = phase - math.floor(phase)

    if isOutputConnected(SIN_OUTPUT) then
        local sin = math.sin(2 * math.pi * phase)
        setVoltage(SIN_OUTPUT, sin * 5)
    end

    if isOutputConnected(TRI_OUTPUT) then
        local tri = 1 - math.min(math.abs(phase - 0.25), math.abs(phase - 1.25)) * 4
        setVoltage(TRI_OUTPUT, tri * 5)
    end

    if isOutputConnected(SAW_OUTPUT) then
        local saw = 2 * phase - 1
        setVoltage(SAW_OUTPUT, saw * 5)
    end

    if isOutputConnected(SQR_OUTPUT) then
        local sqr
        if phase < 0.5 then
            sqr = 1
        else
            sqr = -1
        end
        setVoltage(SQR_OUTPUT, sqr * 5)
    end
end
