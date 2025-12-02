#pragma once
#include <Arduino.h>

// TMC5160 Config
const uint16_t TMC_RMS_CURRENT = 400; // Higher = More torque, more heat. Lower = Cooler, less torque (risk of skipping steps).
const uint8_t TMC_TOFF = 4; // Controls switching frequency. Adjust if motor makes high-pitched noise.
const uint8_t TMC_BLANK_TIME = 24; // Comparator Blank Time in clock cycles (16, 24, 36, 54).
const int TMC_MICROSTEP = 256; // Microstep resolution (1 to 256).
const bool TMC_EN_PWM_MODE = true; // true = StealthChop (Silent, voltage-controlled). false = SpreadCycle (Loud/Powerful, current-controlled).
const bool TMC_PWM_AUTOSCALE = true; // true = Chip adjusts voltage based on load (Recommended). false = Fixed voltage (Risk of stalling).
const uint8_t TMC_RAMPMODE = 1; // 0 = Positioning (Go to Target). 1 = Velocity+ (Spin continuously). 2 = Velocity-. 3 = Hold.
const uint32_t TMC_AMAX = 1000; // Higher = Faster speed-up (snappy). Lower = Slower, smoother build-up of speed.
const uint32_t TMC_DMAX = 1000; // Higher = Faster braking (hard stop). Lower = Slower, smoother stop.
const int TMC_VMAX = 50000; // Higher = Faster braking (hard stop). Lower = Slower, smoother stop.
