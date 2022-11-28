/**
 * Partymatrix - LED Matrix that visualizes music.
 */

#include <Arduino.h> // Arduino standard library.
#include <FastLED.h> // FastLED library.
#include <main.h> // Header file.

static const bool DEBUG_MODE = true; // Debug mode toggle.

static const uint8_t LED_PIN = 7; // Pin for the LEDs.
static const uint8_t MIC_PIN = A3; // Pin for the microphone input.

static const int NUM_LEDS = 150; // Number of LEDs.
static const int LED_POWER_VOLTS = 5; // Volt cap for LEDs.
static const int LED_POWER_MILLIAMPS = 7000; // Ampere cap for LEDs.

// TODO: Add button pin consts and switches.

CRGB leds[NUM_LEDS]; // Initialize the LED array.

int count = 0;
int color = 0;
int colors = 6;

/**
 * Initializing the Partymatrix.
 */
void setup() {
    initFastLED();
    initPins();

    if (isDebug()) {
        Serial.begin(115200);
    }
}

/**
 * Main loop.
 *
 * Contains placeholder test.
 */
void loop() {
    int sound = analogRead(MIC_PIN);

    int numLedsToLight = map(sound, 0, 15, 0, NUM_LEDS); // Map the microphone reading to the amount of LEDs.

    // First, clear the existing led values
    FastLED.clear();

    for (int led = 0; led < numLedsToLight; led++) {
        leds[led] = CRGB::Red;
    }

    FastLED.show();

    if (isDebug()) {
        Serial.println(sound); // Reading from the microphone.
    }

    delay(30);
}

/**
 * Initialize FastLED with the given configuration.
 */
void initFastLED() {
    CFastLED::addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(LED_POWER_VOLTS,
                                           LED_POWER_MILLIAMPS); // Self-regulate the power consumption.
}

/**
 * Initialize Arduino pins.
 */
void initPins() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(MIC_PIN, INPUT);
    // TODO: Add pin modes for buttons and switches.
}

/**
 * Get the debug mode boolean.
 *
 * @return True if debug mode, false if otherwise.
 */
bool isDebug() {
    return DEBUG_MODE;
}
