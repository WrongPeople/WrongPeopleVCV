#pragma once

using namespace rack;


// Lua
#define LUA_DISPLAY_POS mm2px(Vec(0.000, 24.225))
#define LUA_DISPLAY_SIZE mm2px(Vec(50.800, 48.530))
#define LUA_RELOAD_PARAM_POS mm2px(Vec(34.000, 12.640))

static const Vec LUA_SCRIPT_INPUTS_POS[8] = {
    mm2px(Vec(3.520, 82.490)),
    mm2px(Vec(13.680, 82.490)),
    mm2px(Vec(3.520, 92.650)),
    mm2px(Vec(13.680, 92.650)),
    mm2px(Vec(3.520, 102.810)),
    mm2px(Vec(13.680, 102.810)),
    mm2px(Vec(3.520, 112.970)),
    mm2px(Vec(13.680, 112.970)),
};

static const Vec LUA_SCRIPT_OUTPUTS_POS[8] = {
    mm2px(Vec(28.920, 82.490)),
    mm2px(Vec(39.080, 82.490)),
    mm2px(Vec(28.920, 92.650)),
    mm2px(Vec(39.080, 92.650)),
    mm2px(Vec(28.920, 102.810)),
    mm2px(Vec(39.080, 102.810)),
    mm2px(Vec(28.920, 112.970)),
    mm2px(Vec(39.080, 112.970)),
};


// Tourette
#define TOURETTE_DISPLAY_POS mm2px(Vec(0.000, 48.490))
#define TOURETTE_DISPLAY_SIZE mm2px(Vec(35.560, 4.445))
#define TOURETTE_SIG_A_INPUT_POS mm2px(Vec(2.250, 11.370))
#define TOURETTE_SIG_A_OUTPUT_POS mm2px(Vec(25.110, 11.370))
#define TOURETTE_SIG_B_OUTPUT_POS mm2px(Vec(25.110, 24.070))
#define TOURETTE_SIG_B_INPUT_POS mm2px(Vec(2.250, 24.070))
#define TOURETTE_TRIG_INPUT_POS mm2px(Vec(2.250, 38.040))
#define TOURETTE_POLY_PARAM_POS mm2px(Vec(12.781, 16.660))
#define TOURETTE_RAND_PARAM_POS mm2px(Vec(21.671, 90.320))
#define TOURETTE_THRESH_LO_PARAM_POS mm2px(Vec(3.891, 57.300))
#define TOURETTE_THRESH_HI_PARAM_POS mm2px(Vec(21.671, 57.300))
#define TOURETTE_MIN_LEN_PARAM_POS mm2px(Vec(3.891, 73.810))
#define TOURETTE_MAX_LEN_PARAM_POS mm2px(Vec(21.671, 73.810))
#define TOURETTE_ATTACK_PARAM_POS mm2px(Vec(3.891, 106.830))
#define TOURETTE_RELEASE_PARAM_POS mm2px(Vec(21.671, 106.830))
#define TOURETTE_REPEATS_PARAM_POS mm2px(Vec(3.891, 90.320))
#define TOURETTE_STEREO_PARAM_POS mm2px(Vec(26.710, 35.870))

static const Vec TOURETTE_BUF_LIGHTS_POS[16] = {
    mm2px(Vec(8.346, 53.570)),
    mm2px(Vec(9.616, 53.570)),
    mm2px(Vec(10.886, 53.570)),
    mm2px(Vec(12.156, 53.570)),
    mm2px(Vec(13.426, 53.570)),
    mm2px(Vec(14.696, 53.570)),
    mm2px(Vec(15.966, 53.570)),
    mm2px(Vec(17.236, 53.570)),
    mm2px(Vec(18.506, 53.570)),
    mm2px(Vec(19.776, 53.570)),
    mm2px(Vec(21.046, 53.570)),
    mm2px(Vec(22.316, 53.570)),
    mm2px(Vec(23.586, 53.570)),
    mm2px(Vec(24.856, 53.570)),
    mm2px(Vec(26.126, 53.570)),
    mm2px(Vec(27.396, 53.570)),
};


// MIDIPlayer
#define MIDIPLAYER_DISPLAY_POS mm2px(Vec(0.000, 35.790))
#define MIDIPLAYER_DISPLAY_SIZE mm2px(Vec(50.800, 19.050))
#define MIDIPLAYER_PLAY_INPUT_POS mm2px(Vec(2.250, 12.640))
#define MIDIPLAYER_STOP_INPUT_POS mm2px(Vec(2.250, 22.800))
#define MIDIPLAYER_STOP_PARAM_POS mm2px(Vec(13.680, 22.800))
#define MIDIPLAYER_PLAY_PARAM_POS mm2px(Vec(13.680, 12.640))
#define MIDIPLAYER_PLAY_OUTPUT_POS mm2px(Vec(28.920, 12.640))
#define MIDIPLAYER_STOP_OUTPUT_POS mm2px(Vec(28.920, 22.800))
#define MIDIPLAYER_LOOP_PARAM_POS mm2px(Vec(40.350, 12.640))

static const Vec MIDIPLAYER_GATE_OUTPUTS_POS[6] = {
    mm2px(Vec(20.030, 62.170)),
    mm2px(Vec(20.030, 72.330)),
    mm2px(Vec(20.030, 82.490)),
    mm2px(Vec(20.030, 92.650)),
    mm2px(Vec(20.030, 102.810)),
    mm2px(Vec(20.030, 112.970)),
};

static const Vec MIDIPLAYER_CV_OUTPUTS_POS[6] = {
    mm2px(Vec(9.870, 62.170)),
    mm2px(Vec(9.870, 72.330)),
    mm2px(Vec(9.870, 82.490)),
    mm2px(Vec(9.870, 92.650)),
    mm2px(Vec(9.870, 102.810)),
    mm2px(Vec(9.870, 112.970)),
};

static const Vec MIDIPLAYER_VELOCITY_OUTPUTS_POS[6] = {
    mm2px(Vec(40.350, 62.170)),
    mm2px(Vec(40.350, 72.330)),
    mm2px(Vec(40.350, 82.490)),
    mm2px(Vec(40.350, 92.650)),
    mm2px(Vec(40.350, 102.810)),
    mm2px(Vec(40.350, 112.970)),
};

static const Vec MIDIPLAYER_RETRIGGER_OUTPUTS_POS[6] = {
    mm2px(Vec(30.190, 62.170)),
    mm2px(Vec(30.190, 72.330)),
    mm2px(Vec(30.190, 82.490)),
    mm2px(Vec(30.190, 92.650)),
    mm2px(Vec(30.190, 102.810)),
    mm2px(Vec(30.190, 112.970)),
};

static const Vec MIDIPLAYER_TRACK_LIGHTS_POS[6] = {
    mm2px(Vec(2.722, 65.182)),
    mm2px(Vec(2.722, 75.342)),
    mm2px(Vec(2.722, 85.502)),
    mm2px(Vec(2.722, 95.662)),
    mm2px(Vec(2.722, 105.822)),
    mm2px(Vec(2.722, 115.982)),
};


