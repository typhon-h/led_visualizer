#ifndef LED_H
#define LED_H

// FastLED
#define FASTLED_INTERNAL // Removes pragma message on compile
#include "FastLED.h"
#define LED_PIN 6
#define LED_COUNT 74

#define DELTA_BRIGHTNESS 10
#define DEFAULT_INTENSITY 255
#define DEFAULT_SATURATION 255
#define DEFAULT_STATIC_COLOR CHSV(0, DEFAULT_SATURATION, DEFAULT_INTENSITY)
#define BLANK_LED CHSV(0, 0, 0)

typedef enum
{
  STATIC = 1,    // Solid color
  CYCLE,         // Timed rainbow cycle
  GRADIENT,      // Custom gradient from primary to secondary color
  RAINBOW        // Rainbow gradient
} COLOR_STATE_t; // Gradient selected + gradient select

static COLOR_STATE_t active_state = STATIC; // Default mode
static int intensity = DEFAULT_INTENSITY;
static uint8_t active_hue = 0;
static CHSV gradient_primary = BLANK_LED;
static CHSV gradient_secondary = BLANK_LED;

CRGB leds[LED_COUNT];

void led_setup(void);
void responsive_fade(void);
void color_cycle(void);
void intensity_increase(void);
void intensity_decrease(void);
void set_static_color(CHSV active_color);
void set_gradient(CHSV color);
CRGB get_gradient_value(uint8_t numLEDs, uint8_t index);
CHSV get_rainbow_value(int8_t numLEDs, int8_t index);

#endif
