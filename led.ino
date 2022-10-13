#include "led.h"
#include "ir.h"

CHSV get_rainbow_value(int8_t numLEDs, int8_t index)
{
  uint8_t initialhue = 0;
  uint8_t deltahue = 255 / numLEDs;
  CHSV hsv;

  hsv.hue = initialhue;
  hsv.val = 255;
  hsv.sat = 240;
  hsv.hue = deltahue * index;

  return hsv;
}

CRGB get_gradient_value(uint8_t numLEDs, uint8_t index)
{
  uint8_t startpos = 0;
  uint8_t endpos = numLEDs;

  CRGB startcolor = gradient_primary;
  CRGB endcolor = gradient_secondary;
  // if the points are in the wrong order, straighten them
  if (endpos < startpos)
  {
    uint16_t t = endpos;
    CRGB tc = endcolor;
    endcolor = startcolor;
    endpos = startpos;
    startpos = t;
    startcolor = tc;
  }

  saccum87 rdistance87;
  saccum87 gdistance87;
  saccum87 bdistance87;

  rdistance87 = (endcolor.r - startcolor.r) << 7;
  gdistance87 = (endcolor.g - startcolor.g) << 7;
  bdistance87 = (endcolor.b - startcolor.b) << 7;

  uint16_t pixeldistance = endpos - startpos;
  int16_t divisor = pixeldistance ? pixeldistance : 1;

  saccum87 rdelta87 = rdistance87 / divisor;
  saccum87 gdelta87 = gdistance87 / divisor;
  saccum87 bdelta87 = bdistance87 / divisor;

  rdelta87 *= 2;
  gdelta87 *= 2;
  bdelta87 *= 2;

  accum88 r88 = startcolor.r << 8;
  accum88 g88 = startcolor.g << 8;
  accum88 b88 = startcolor.b << 8;

  r88 += rdelta87 * index;
  g88 += gdelta87 * index;
  b88 += bdelta87 * index;

  return CRGB(r88 >> 8, g88 >> 8, b88 >> 8);
}

void led_setup()
{
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(intensity);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();
}

void color_cycle()
{
  active_hue++;
  if (!responsive_mode)
  {
    fill_solid(leds, LED_COUNT, CHSV(active_hue++, DEFAULT_SATURATION, DEFAULT_INTENSITY));
    FastLED.show();
  }
}

void intensity_increase()
{
  intensity += DELTA_BRIGHTNESS;
  if (intensity >= 255)
  {
    intensity = 255;
  }
  FastLED.setBrightness(intensity);
  FastLED.show();
}

void intensity_decrease()
{
  intensity -= DELTA_BRIGHTNESS;
  if (intensity <= DELTA_BRIGHTNESS)
  {
    intensity = 5;
  }

  FastLED.setBrightness(intensity);
  FastLED.show();
}

void set_static_color(uint8_t hue)
{
  active_hue = hue;
  if (!responsive_mode)
  {
    fill_solid(leds, LED_COUNT, CHSV(active_hue, DEFAULT_SATURATION, DEFAULT_INTENSITY));
    FastLED.show();
  }
}

void set_gradient(uint8_t hue)
{ // Requires two calls to set gradient
  CHSV color = CHSV(hue, DEFAULT_SATURATION, DEFAULT_INTENSITY);

  if (gradient_primary == BLANK_LED)
  {
    gradient_primary = color;

    if (!responsive_mode)
    {
      fill_gradient(leds, 0, gradient_primary, LED_COUNT / 2, BLANK_LED);
      fill_solid(leds + LED_COUNT / 2, LED_COUNT / 2, BLANK_LED);
      FastLED.show();
    }
  }

  if (gradient_secondary == BLANK_LED && color != gradient_primary)
  {
    gradient_secondary = color;
    if (!responsive_mode)
    {
      fill_gradient(leds, 0, gradient_primary, LED_COUNT, gradient_secondary);
      FastLED.show();
    }
  }

  // if (gradient_primary != BLANK_LED && gradient_secondary != BLANK_LED) {
  //
  //
  // }
}
