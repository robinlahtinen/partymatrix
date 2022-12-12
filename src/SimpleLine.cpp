/**
 * Visualization: Simple Line
 */

#include <Arduino.h> // Arduino standard library.
#include "FastLED.h" // FastLED library.
#include "main.h" // Main header file.
#include "SimpleLine.h" // Header file.

int lineMaxStrength = 100;
int lastStrength = 0;
int resetCount = 0;

void doVisualizeSimpleLine(int strength) {
    if (strength >= lastStrength) {
        lastStrength = strength;
    } else if (lastStrength > 0) {
        lastStrength--;
    }

    if (lastStrength > lineMaxStrength) {
        lineMaxStrength = strength;
    } else {
        EVERY_N_MILLIS(500) {
            lineMaxStrength--;
        }
    }

    if (lastStrength <= 25) {
        EVERY_N_MILLIS(500) {
            if (resetCount <= 10) {
                resetCount++;
            }
        }
    } else if (resetCount > 0) {
        resetCount--;
    }

    int numLedsToLight = map(lastStrength, 0, lineMaxStrength, 0,
                             NUM_LEDS); // Map the microphone reading to the amount of LEDs.

    for (int led = 0; led < NUM_LEDS; led++) {
        if (led < numLedsToLight && resetCount < 10) {
            int colorAmount = map(led, 0, numLedsToLight, 0, 255);

            if (strength > 10) {
                leds[led] = blend(CRGB::Green, CRGB::Red, colorAmount);
            }

            leds[led].fadeLightBy(colorAmount);
        } else {
            fadeToBlackBy(&leds[led], 1, 200);
        }
    }

    FastLED.show();
}
