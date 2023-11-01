# HC_SR04

## Description
The original version is from [SquachWare hc_sr04](https://github.com/skizzophrenic/SquachWare-CFW/tree/dev/applications/plugins/hc_sr04).

This version modifies the code to use furi_hal_cortex_timer_get(0) to
obtain a high precision counter.  We also tune the calculations based on measured observations.  The display shows 2 digits after the decimal point, displaying in both cm and inches.

## Pins
- (Flipper -> HC_SR04 device)
- (5V -> VCC)
- (GND -> GND)
- (13|TX -> Trig)
- (14|RX -> Echo)

## Code changes
- #include <furi_hal_cortex.h>
  - This has the definitions for FuriHalCortexTimer and furi_hal_cortex_timer_get(...)
- FuriHalCortexTimer beginTimer = furi_hal_cortex_timer_get(0);
  - This gets the current time in beginTimer.start.
  - The offset is 0 (we could use offset ).
- furi_string_printf(str_buf, "Distance: %0.2f cm", (double)plugin_state->distance);
  - This rounds the output to the nearest hundredths place.
- hc_sr04_duration_to_cm(...)
  - I have no idea why total_dist needs to be multiplied by 1.588, but doing so seems to make all of the measurements correct.
  - The code overhead, delays in IN/OUT, etc. seem to take the same time as sound tranvelling for 0.497 cm there and back; so we add this fixed constant.  If you Flipper was faster/slower you would need to adjust this constant.
