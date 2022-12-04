//
// Created by Robin on 4.12.2022.
//

#include <Arduino.h> // Arduino standard library.
#include <FastLED.h> // FastLED library.
#include "SimpleSpectrum.h"

#define kMatrixHeight 24
#define kMatrixWidth 8
#define NUM_LEDS kMatrixHeight*kMatrixWidth    //192

CRGB leds[NUM_LEDS];

#define LED_PIN                           2                                   // LED strip data
#define COLOR_ORDER                       GRB                                 // If colours look wrong, play with this
#define CHIPSET                           WS2812B                             // LED strip type
#define currentBrightness                 100


uint16_t spectrumAvg;   //contains audio amplitude value

uint8_t spectrumByteSpeed[kMatrixWidth];
uint8_t spectrumByteMin[kMatrixWidth];
uint8_t spectrumByteMax[kMatrixWidth];
uint8_t spectrumByteVal[kMatrixWidth];

uint8_t spbPeakDelay = 100;   // higher value = slower peak decay
uint8_t spbPeakHold = 20;     // higher value = longer peak hold before decay

#define VUGRAVITY 1
uint8_t spbShiftHue = 0;
uint8_t spbSR[] = {0,0,0,0,0,0,0,0};
uint8_t spbOldSR[] = {0,0,0,0,0,0,0,0};
uint8_t peakValue[] = {0,0,0,0,0,0,0,0};
static uint8_t topLED[8] = {0,0,0,0,0,0,0,0};
static uint8_t peakHold[8] = {0,0,0,0,0,0,0,0};
bool spectrumByteReset = true;


void readAudio()
{
    // code not written, many example of read audio codes available elsewhere
    // to read audio, result is contained in spectrumAvg
}


void initSpectrumByte(uint8_t speedLo, uint8_t speedHi, uint8_t minLo, uint8_t minHi, uint8_t maxLo, uint8_t maxHi)
{
    random16_set_seed(65535);
    random16_add_entropy(analogRead(32));
    for (uint8_t i=0; i<kMatrixWidth; i++)
    {
        spectrumByteSpeed[i] = random(speedLo, speedHi);
        spectrumByteMin[i] = random(minLo, minHi);
        spectrumByteMax[i] = random(maxLo, maxHi);
        spbSR[i] = 0;
        spbOldSR[i]=0;
    }
    spectrumByteReset = false;
}


void spectrumbars()
{
    uint8_t peakHeight;
    CRGB colorVU, colorPeak;

    EVERY_N_MILLIS(60)
    {
        spbShiftHue = addmod8(spbShiftHue, 1, 255);
    }

    if (spectrumByteReset) initSpectrumByte(10, 40, 5, 70, 120, 255);
    fadeToBlackBy(leds, NUM_LEDS, 200);

    static int gravityCounter = 0;

    for (uint8_t i = 0; i < kMatrixWidth; i++)
    {
        spectrumByteVal[i] = beatsin8(spectrumByteSpeed[i], spectrumByteMin[i], constrain(spectrumByteMax[i], 0, 255));
        spbSR[i] = spectrumByteVal[i] * (float)(0.0085 * spectrumAvg);
        // spectrumAvg = single variable no FFT, represent audio amplitude value
        // tweak the value of 0.0085 to achieve good displayed result
        spbSR[i] = ((spbOldSR[i]*6)+spbSR[i])/7;

        uint8_t vuValue = map(spbSR[i], 0, 255, 1, kMatrixHeight);
        for (uint8_t j = 0; j < vuValue; j++)
        {
            colorVU = ColorFromPalette(RainbowColors_p, ((i * kMatrixHeight)+j-spbShiftHue) % 255, 255, LINEARBLEND);
            colorPeak = blend(colorVU, CRGB(200,200,200), 128);
            leds[XY(i,j)] = colorVU;
        }

        if (vuValue >= topLED[i]) { topLED[i] = vuValue; peakHold[i] = 0; }
        else if (gravityCounter % VUGRAVITY == 0)
        {
            EVERY_N_MILLIS_I(timeObj, spbPeakDelay)
            {
                timeObj.setPeriod(spbPeakDelay);
                for (uint8_t x=0; x<kMatrixWidth; x++) { if (peakHold[x] > spbPeakHold) topLED[x]--; }    // peak decay
            }
        }

        if (topLED[i] > 0) leds[XY(i, topLED[i])] = colorPeak;   // set position of peak leds
    }

    gravityCounter = (gravityCounter + 1) % VUGRAVITY;

    for (uint8_t x=0; x<kMatrixWidth; x++) spbOldSR[x] = spbSR[x];
    EVERY_N_MILLIS(16)
    {
        for (uint8_t x=0; x<kMatrixWidth; x++) peakHold[x]++;
    }

    FastLED.show();
}

uint16_t XY( uint8_t x, uint8_t y)
{
    if(y >= kMatrixHeight) { y = kMatrixHeight - 1; }
    if(x >= kMatrixWidth) { x = kMatrixWidth - 1; }

    return (x * kMatrixHeight) + y;
    //this is for vertical matrix, led0 on bottom left, non-serpentine.
    //More info, check https://macetech.github.io/FastLED-XY-Map-Generator/
}

/**
 * Visualize a simple fake audio spectrum.
 *
 * @param strength Strength of the visualization.
 */
void doVisualizeSimpleSpectrum(int strength) {

}
