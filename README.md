# Wrong People modules

Modules for [VCV Rack](https://vcvrack.com/) 1.x.

![screenshot](./screenshots/all.jpg)


## Modules


### MIDI Player

Multitrack MIDI files player.


### Lua

Lua interpreter with 8 inputs, 8 outputs and display.

See [res/lua](./res/lua/api.lua) for available functions and classes.

See also an [examples](./res/lua/examples).


### Tourette

Tourette syndrome simulator.

When the signal level reaches the “High Threshold” value, a recording is started in the first of free of 16 buffers.
When the signal level drops to “Low Threshold”, the recording stops.

The values of the parameters "Min shot length" and "Max shot length" are taken modulo, while the sign (plus or minus) determines the behavior during the recording.

When the signal level drops to "Low Threshold" and the recording length is less than "Min shot length" then:
* if "Min shot length" has a negative value, the recording is cancelled and the buffer remains free.
* if the parameter value is positive, recording continues until the value drops to "Low Threshold" again and the recording length reaches or exceeds "Min shot length".

When reaching the record length "Max shot length" then:
* if the parameter value is negative, the record is cancelled and the buffer remains free.
* if positive, recording stops and will be saved in the buffer.

When a pulse is sent to the input "Trig" a playback starts from the first available buffer.

The parameter "Randomness" indicates the probability that the buffers will be shuffled.

The "Repeats" parameter indicates how many times the contents of each buffer will be played.
After reaching the number of plays, the buffer will be cleared and becomes writable again.

The parameter "Max polyphony" indicates how many buffers can be played simultaneously.

The "Attack" and "Release" parameters define the envelope during playback.

In "Stereo" mode, the input signal is processed as a stereo signal.
In "Split" mode, the input signal is processed as 2 mono signals.

The display shows the input levels and recording status.
The 16 LEDs below the display indicate the status of the buffers:
* off - the buffer is free and writable
* red - writing to the buffer
* blue - the buffer is available for playback
* green - playback from the buffer is in progress



## Building

[Set up](https://vcvrack.com/manual/Building.html#setting-up-your-development-environment) your build environment, [download](https://vcvrack.com/downloads/) and extract the latest Rack SDK, then:

```
git clone https://github.com/WrongPeople/WrongPeopleVCV.git
cd WrongPeopleVCV
git submodule update --init --recursive
RACK_DIR=path/to/Rack-SDK make
```
