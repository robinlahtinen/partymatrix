/**
 * Visualization: Simple Spectrum
 */

#include <Arduino.h> // Arduino standard library.
#include "FastLED.h" // FastLED library.
#include "main.h" // Main header file.
#include "SimpleSpectrum.h" // Header file.

#define kMatrixHeight 12
#define kMatrixWidth 12
#define LAST_VISIBLE_LED 143

uint16_t spectrumAvg;   //contains audio amplitude value

uint16_t spectrumByteSpeed[kMatrixWidth];
uint16_t spectrumByteMin[kMatrixWidth];
uint16_t spectrumByteMax[kMatrixWidth];
uint16_t spectrumByteVal[kMatrixWidth];

uint16_t spbPeakDelay = 100;   // higher value = slower peak decay
uint16_t spbPeakHold = 20;     // higher value = longer peak hold before decay

#define VUGRAVITY 1
uint16_t spbShiftHue = 0;
uint16_t spbSR[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t spbOldSR[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t peakValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint16_t topLED[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint16_t peakHold[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool spectrumByteReset = true;

void
initSpectrumByte(uint16_t speedLo, uint16_t speedHi, uint16_t minLo, uint16_t minHi, uint16_t maxLo, uint16_t maxHi) {
    random16_set_seed(65535);
    random16_add_entropy(analogRead(0));
    for (uint16_t i = 0; i < kMatrixWidth; i++) {
        spectrumByteSpeed[i] = random(speedLo, speedHi);
        spectrumByteMin[i] = random(minLo, minHi);
        spectrumByteMax[i] = random(maxLo, maxHi);
        spbSR[i] = 0;
        spbOldSR[i] = 0;
    }
    spectrumByteReset = false;
}


void spectrumbars() {
    uint16_t peakHeight;
    CRGB colorVU, colorPeak;

    EVERY_N_MILLIS(60)
    {
        spbShiftHue = addmod8(spbShiftHue, 1, 255);
    }

    if (spectrumByteReset) initSpectrumByte(10, 40, 5, 70, 120, 255);
    fadeToBlackBy(leds, NUM_LEDS, 200);

    static int gravityCounter = 0;

    for (uint16_t i = 0; i < kMatrixWidth; i++) {
        spectrumByteVal[i] = beatsin8(spectrumByteSpeed[i], spectrumByteMin[i], constrain(spectrumByteMax[i], 0, 255));
        spbSR[i] = spectrumByteVal[i] * (float) (0.075 * spectrumAvg);
        // spectrumAvg = single variable no FFT, represent audio amplitude value
        // tweak the value of 0.0085 to achieve good displayed result
        spbSR[i] = ((spbOldSR[i] * 6) + spbSR[i]) / 7;

        uint16_t vuValue = map(spbSR[i], 0, 255, 1, kMatrixHeight);
        for (uint16_t j = 0; j < vuValue; j++) {
            colorVU = ColorFromPalette(RainbowColors_p, ((i * kMatrixHeight) + j - spbShiftHue) % 255, 255,
                                       LINEARBLEND);
            colorPeak = blend(colorVU, CRGB(200, 200, 200), 128);
            leds[XY(i, j)] = colorVU;
        }

        if (vuValue >= topLED[i]) {
            topLED[i] = vuValue;
            peakHold[i] = 0;
        } else if (gravityCounter % VUGRAVITY == 0) {
            EVERY_N_MILLIS_I(timeObj, spbPeakDelay) {
                timeObj.setPeriod(spbPeakDelay);
                for (uint16_t x = 0; x < kMatrixWidth; x++) {
                    if (peakHold[x] > spbPeakHold)
                        topLED[x]--;
                }    // peak decay
            }
        }

        if (topLED[i] > 0) leds[XY(i, topLED[i])] = colorPeak;   // set position of peak leds
    }

    gravityCounter = (gravityCounter + 1) % VUGRAVITY;

    for (uint16_t x = 0; x < kMatrixWidth; x++) spbOldSR[x] = spbSR[x];
    EVERY_N_MILLIS(16)
    {
        for (uint16_t x = 0; x < kMatrixWidth; x++) peakHold[x]++;
    }

    FastLED.show();
}

/**
 * Automatically generated XYmap function for irregular/gapped LED arrays.
 * Source: https://macetech.github.io/FastLED-XY-Map-Generator/
 *
 * @param x
 * @param y
 * @return
 */
uint16_t XY(uint16_t x, uint16_t y) {
    // any out of bounds address maps to the first hidden pixel
    if ((x >= kMatrixWidth) || (y >= kMatrixHeight)) {
        return (LAST_VISIBLE_LED + 1);
    }

    const uint16_t XYTable[] = {
            143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132,
            120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
            119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108,
            96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
            95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84,
            72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
            71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60,
            48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
            47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36,
            24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
            23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
    };


    uint16_t i = (y * kMatrixWidth) + x;
    uint16_t j = XYTable[i];
    return j;
}

/**
 * Visualize a simple fake audio spectrum.
 *
 * @param strength Strength of the visualization.
 */
void doVisualizeSimpleSpectrum(int strength) {
    spectrumAvg = strength;

    spectrumbars();
}
