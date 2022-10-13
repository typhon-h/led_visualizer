#include "led.h"

void cpyBuffer(CRGB* src, CRGB* dest) {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    dest[i] = src[i];
  }
}

void led_setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(intensity);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();
}

void color_cycle() {
  static uint8_t hue = 0;
  fill_solid(leds_buffer, LED_COUNT, CHSV(hue++, DEFAULT_SATURATION, DEFAULT_INTENSITY));
  if (!responsive_mode) {
    cpyBuffer(leds_buffer, leds);
    FastLED.show();
  }
}

void intensity_increase() {
  intensity += DELTA_BRIGHTNESS;
  if (intensity >= 255) {
    intensity = 255;
  }
  FastLED.setBrightness(intensity);
  FastLED.show();
}

void intensity_decrease() {
  intensity -= DELTA_BRIGHTNESS;
  if (intensity <= DELTA_BRIGHTNESS) {
    intensity = 5;
  }
  FastLED.setBrightness(intensity);
  FastLED.show();
}

void set_static_color(CHSV active_color) {
  fill_solid(leds_buffer, LED_COUNT, active_color);
  if (!responsive_mode) {
    cpyBuffer(leds_buffer, leds);
    FastLED.show();
  }

}

void set_gradient(CHSV color) { // Requires two calls to set gradient
  static CHSV primary = BLANK_LED;
  static CHSV secondary = BLANK_LED;

  if (primary == BLANK_LED) {
    primary = color;
    fill_gradient(leds_buffer, 0, primary, LED_COUNT / 2, BLANK_LED);
    fill_solid(leds_buffer + LED_COUNT / 2, LED_COUNT / 2, BLANK_LED);
    if (!responsive_mode) {
      cpyBuffer(leds_buffer, leds);
      FastLED.show();
    }
  }

  if (secondary == BLANK_LED && color != primary) {
    secondary = color;
  }

  if (primary != BLANK_LED && secondary != BLANK_LED) {
    fill_gradient(leds_buffer, 0, primary, LED_COUNT, secondary);
    leds[0] = CRGB(1,2,3);
    if (!responsive_mode) {
      cpyBuffer(leds_buffer, leds);
      FastLED.show();
    }

    primary = secondary = BLANK_LED;
  }
}
