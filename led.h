#ifndef LED_H
#define LED_H

//FastLED
#define FASTLED_INTERNAL //Removes pragma message on compile
#include "FastLED.h"
#define LED_PIN 6
#define LED_COUNT 74

#define DELTA_BRIGHTNESS 10
#define DEFAULT_INTENSITY 255
#define DEFAULT_SATURATION 255
#define DEFAULT_STATIC_COLOR CHSV(0, DEFAULT_SATURATION, DEFAULT_INTENSITY)
#define BLANK_LED CHSV(0, 0, 0)

typedef enum {
  STATIC = 1, //Solid color
  CYCLE, //Timed rainbow cycle
  GRADIENT, //Custom gradient from primary to secondary color
  RAINBOW //Rainbow gradient
} COLOR_STATE_t;

static COLOR_STATE_t active_state = STATIC; //Default mode
static int intensity = DEFAULT_INTENSITY;

CRGB leds[LED_COUNT];
CRGB leds_buffer[LED_COUNT];

void cpyBuffer(CRGB* src, CRGB* dest);
void led_setup(void);
void responsive_fade(void);
void color_cycle(void);
void intensity_increase(void);
void intensity_decrease(void);
void set_static_color(CHSV active_color);
void set_gradient(CHSV color);


#endif
