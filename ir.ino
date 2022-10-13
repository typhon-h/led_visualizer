#include "ir.h"
#include "led.h"

static bool power_on = true;
static bool responsive_mode = false;
static bool responsive_intensity = false;
static int count = 15;

void ir_setup()
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
}

void ir_poll()
{
  if (IrReceiver.decode())
  {
    if (IrReceiver.decodedIRData.protocol == 8) // NEC
    {
      ir_run_command(IrReceiver.decodedIRData.command);
    }
    IrReceiver.resume();
  }
  Serial.print(IrReceiver.decodedIRData.command);
  Serial.print(' ');
  // Serial.print(power_on);
  Serial.print(' ');
  Serial.println(intensity);
}

void strip_on()
{
  switch (active_state)
  {
  case GRADIENT:
    fill_gradient(leds, 0, gradient_primary, LED_COUNT, gradient_secondary);
    break;
  case RAINBOW:
    fill_rainbow(leds, LED_COUNT, 0, 255 / LED_COUNT);
    break;
  default:
    fill_solid(leds, LED_COUNT, CHSV(active_hue++, DEFAULT_SATURATION, DEFAULT_INTENSITY));
    break;
  }
}

void ir_run_command(int command)
{
  if (!power_on && command == 0x03)
  { // 0x03: Power
    power_on = true;
    FastLED.setBrightness(intensity);
    strip_on();
    FastLED.show();
    if (active_state == CYCLE)
    {
      cycle_task.enable();
    }
  }
  else
  { // Only register other buttons if power on
    switch (command)
    {
    case 0x00: // Intensity Increase
      if (!responsive_intensity)
      {
        intensity_increase();
      }

      break;

    case 0x01: // Intensity Decrease
      if (!responsive_intensity)
      {
        intensity_decrease();
      }
      break;

    case 0x02: // Responsive Intensity
      if (responsive_mode)
      {
        responsive_intensity = !responsive_intensity;
      }
      break;

    case 0x03: // Power
      power_on = false;
      FastLED.setBrightness(0);
      FastLED.clear();
      FastLED.show();

      cycle_task.disable();
      break;

    case 0x04:
      switch (active_state)
      {
      case STATIC:
        set_static_color(0);
        break;
      case GRADIENT:
        set_gradient(0);
        break;
      default:
        break;
      }

      break;

    case 0x05:
      switch (active_state)
      {
      case STATIC:
        set_static_color(85);
        break;
      case GRADIENT:
        set_gradient(85);
        break;
      default:
        break;
      }

      break;

    case 0x06:
      switch (active_state)
      {
      case STATIC:
        set_static_color(170);
        break;
      case GRADIENT:
        set_gradient(170);
        break;
      default:
        break;
      }

      break;

    case 0x07: // Rainbow state
      active_state = RAINBOW;
      cycle_task.disable(); // CALCULATE GRADIENT

      if (!responsive_mode)
      {
        fill_rainbow(leds, LED_COUNT, 0, 255 / LED_COUNT);
        FastLED.show();
      }

      break;

    case 0x08:
      switch (active_state)
      {
      case STATIC:
        set_static_color(17);
        break;
      case GRADIENT:
        set_gradient(17);
        break;
      default:
        break;
      }

      break;

    case 0x09:
      switch (active_state)
      {
      case STATIC:
        set_static_color(102);
        break;
      case GRADIENT:
        set_gradient(102);
        break;
      default:
        break;
      }

      break;

    case 0x0A:
      switch (active_state)
      {
      case STATIC:
        set_static_color(187);
        break;
      case GRADIENT:
        set_gradient(187);
        break;
      default:
        break;
      }

      break;

    case 0x0B: // Static state
      active_state = STATIC;
      cycle_task.disable();
      set_static_color(active_hue);
      break;

    case 0x0C:
      switch (active_state)
      {
      case STATIC:
        set_static_color(34);
        break;
      case GRADIENT:
        set_gradient(34);
        break;
      default:
        break;
      }

      break;

    case 0x0D:
      switch (active_state)
      {
      case STATIC:
        set_static_color(119);
        break;
      case GRADIENT:
        set_gradient(119);
        break;
      default:
        break;
      }

      break;

    case 0x0E:
      switch (active_state)
      {
      case STATIC:
        set_static_color(204);
        break;
      case GRADIENT:
        set_gradient(204);
        break;
      default:
        break;
      }

      break;

    case 0x0F: // Gradient state
      active_state = GRADIENT;
      cycle_task.disable();
      FastLED.clear();
      FastLED.show();
      gradient_primary = gradient_secondary = BLANK_LED;
      break;

    case 0x10:
      switch (active_state)
      {
      case STATIC:
        set_static_color(51);
        break;
      case GRADIENT:
        set_gradient(51);
        break;
      default:
        break;
      }

      break;

    case 0x11:
      switch (active_state)
      {
      case STATIC:
        set_static_color(136);
        break;
      case GRADIENT:
        set_gradient(136);
        break;
      default:
        break;
      }

      break;

    case 0x12:
      switch (active_state)
      {
      case STATIC:
        set_static_color(221);
        break;
      case GRADIENT:
        set_gradient(221);
        break;
      default:
        break;
      }

      break;

    case 0x13: // Cycle state
      active_state = CYCLE;
      cycle_task.enable();
      break;

    case 0x14:
      switch (active_state)
      {
      case STATIC:
        set_static_color(68);
        break;
      case GRADIENT:
        set_gradient(68);
        break;
      default:
        break;
      }

      break;

    case 0x15:
      switch (active_state)
      {
      case STATIC:
        set_static_color(153);
        break;
      case GRADIENT:
        set_gradient(153);
        break;
      default:
        break;
      }

      break;

    case 0x16:
      switch (active_state)
      {
      case STATIC:
        set_static_color(238);
        break;
      case GRADIENT:
        set_gradient(238);
        break;
      default:
        break;
      }

      break;

    case 0x17: // Responsive Mode
      responsive_mode = !responsive_mode;
      if (responsive_mode)
      {
        FastLED.clear();
        FastLED.show();
        midi_task.enable();
      }
      else
      {
        strip_on();
        FastLED.show();
        midi_task.disable();
        responsive_intensity = false;
      }
      break;

    default:
      break;
    }
  }
}