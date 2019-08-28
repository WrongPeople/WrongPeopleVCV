setDisplayMode("scope")

--[[
for i = 0, 3 do
    showValue(i)
end
--]]

function process()
    scopeTrigValue = -1

    for i = 0, 3 do
        if isInputConnected(i) then
            if scopeTrigValue == -1 then
                scopeTrigValue = i
                setScopeTrigValue(i)
            end
            showValue(i)
            setValue(i, getVoltage(i))
        else
            hideValue(i)
        end
    end

end
