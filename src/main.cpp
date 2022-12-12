/**
 * Partymatrix - LED Matrix that visualizes music.
 */

#include <Arduino.h> // Arduino standard library.
#include <FastLED.h> // FastLED library.
#include <main.h> // Header file.
#include "SimpleSpectrum.h" // Visualization: Simple Spectrum.
#include "SimpleLine.h" // Visualization: Simple Line.

static const bool DEBUG_MODE = true; // Debug mode toggle.

static const uint8_t LED_PIN = 7; // Pin for the LEDs.
static const uint8_t MIC_PIN = A3; // Pin for the microphone input.
static const uint8_t SWITCH_PIN = 3; // Pin for the microphone switch.
static const uint8_t BTN1_PIN = 6; // Pin for the button 1.
static const uint8_t BTN2_PIN = 5; // Pin for the button 2.

extern const int NUM_LEDS = 144; // Number of LEDs.
static const int LED_POWER_VOLTS = 5; // Volt cap for LEDs.
static const int LED_POWER_MILLIAMPS = 8000; // Ampere cap for LEDs.
static const int VISUALIZATIONS = 2; // Amount of available visualizations.

/**
 * List of available visualizations.
 */
enum Visualization {
    VIS_SIMPLESPECTRUM,
    VIS_SIMPLELINE,
};

CRGB leds[NUM_LEDS]; // Initialize the LED array.

bool micSimStatus = false; // Is microphone simulated.
int micReading; // Last microphone reading.
int selectedVisualization = VIS_SIMPLESPECTRUM; // Selected visualization.

/**
 * Button state booleans.
 */
bool btn1LastState, btn1CurrentState, btn2LastState, btn2CurrentState;

/**
 * Initializing the Partymatrix.
 */
void setup() {
    initFastLED();
    initPins();
    initButtons();

    if (isDebug()) {
        Serial.begin(115200);
    }
}

/**
 * Main loop.
 */
void loop() {
    micReading = getMicReading();

    doButtonUpdate();

    switch (selectedVisualization) {
        case VIS_SIMPLELINE:
            doVisualizeSimpleLine(micReading);
            break;
        default:
            doVisualizeSimpleSpectrum(micReading);
            break;
    }

    doDebugPrint();
}

/**
 * Initialize FastLED with the given configuration.
 */
void initFastLED() {
    CFastLED::addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(LED_POWER_VOLTS,
                                           LED_POWER_MILLIAMPS); // Self-regulate the power consumption.
    FastLED.setBrightness(255);
}

/**
 * Initialize Arduino pins.
 */
void initPins() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(MIC_PIN, INPUT);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
}

/**
 * Initialize button states.
 */
void initButtons() {
    btn1CurrentState = digitalRead(BTN1_PIN);
    btn2CurrentState = digitalRead(BTN2_PIN);
}

/**
 * Get the debug mode boolean.
 *
 * @return True if debug mode, false if otherwise.
 */
bool isDebug() {
    return DEBUG_MODE;
}

/**
 * Get the microphone level from simulation or using a real microphone.
 *
 * @return Microphone volume strength.
 */
int getMicReading() {
    if (digitalRead(SWITCH_PIN) == LOW) {
        return getSimulatedMicReading();
    }

    return analogRead(MIC_PIN);
}

/**
 * Exaggrated microphone reading simulation.
 *
 * @return Microphone reading.
 */
int getSimulatedMicReading() {
    if (micReading >= 150) {
        micSimStatus = false;
    } else if (micReading <= 0) {
        micSimStatus = true;
    } else if (50 <= random8(100)) {
        micSimStatus = !micSimStatus;
    }

    EVERY_N_MILLIS(random16(800, 2500)) {
        if (micSimStatus) {
            micReading = micReading + random8(1, 50);
        } else {
            micReading = micReading - random8(2, 100);
        }
    }

    EVERY_N_MILLIS(40) {
        micReading--;
    }

    if (micReading < 0) {
        micReading = 0;
    }

    return micReading;
}

/**
 * Reset the leds for a clear matrix.
 */
void doResetLeds() {
    FastLED.clear(true);
}

/**
 * Update button states.
 */
void doButtonUpdate() {
    btn1LastState = btn1CurrentState;
    btn2LastState = btn2CurrentState;

    btn1CurrentState = digitalRead(BTN1_PIN);
    btn2CurrentState = digitalRead(BTN2_PIN);

    if (btn1LastState == HIGH && btn1CurrentState == LOW && selectedVisualization < (VISUALIZATIONS - 1)) {
        selectedVisualization++;
        doResetLeds();
    } else if (btn2LastState == HIGH && btn2CurrentState == LOW && selectedVisualization > 0) {
        selectedVisualization--;
        doResetLeds();
    }
}

/**
 * Print informational debug messages.
 */
void doDebugPrint() {
    if (isDebug()) {
        EVERY_N_MILLIS(100) {
            // Reading from the microphone.
            Serial.print("MIC READ: ");
            Serial.println(micReading);

            // Reading from the switch pin.
            Serial.print("SWITCH READ: ");
            Serial.println(digitalRead(SWITCH_PIN));

            // Reading from the button 1 pin.
            Serial.print("BTN1 READ: ");
            Serial.println(digitalRead(BTN1_PIN) ? "OFF" : "PRESSING");

            // Reading from the button 2 pin.
            Serial.print("BTN2 READ: ");
            Serial.println(digitalRead(BTN2_PIN) ? "OFF" : "PRESSING");

            // Visualization status
            Serial.print("VISUALIZATION: ");
            Serial.print(selectedVisualization + 1);
            Serial.print("/");
            Serial.println(VISUALIZATIONS);
        }
    }
}
