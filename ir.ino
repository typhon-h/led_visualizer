#include "ir.h"
#include "led.h"

static bool power_on = true;
static bool responsive_mode = false;
static bool responsive_intensity = false;

void ir_setup() {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
}

void ir_poll() {
  if (IrReceiver.decode()) {
    ir_run_command(IrReceiver.decodedIRData.command);
    IrReceiver.resume();
  }
  Serial.print(IrReceiver.decodedIRData.command);
  Serial.print(' ');
  Serial.print(power_on);
  Serial.print(' ');
  Serial.println(intensity);
}

void ir_run_command(int command) {
  if (!power_on && command == 0x03) { //0x03: Power
    power_on = true;
    FastLED.setBrightness(intensity);
    cpyBuffer(leds_buffer, leds);
    FastLED.show();
    if (active_state == CYCLE) {
      cycle_task.enable();
    }

  }
  else { // Only register other buttons if power on
    switch (command) {
      case 0x00: // Intensity Increase
        if (!responsive_intensity) {
          intensity_increase();
        }
        break;

      case 0x01: // Intensity Decrease
        if (!responsive_intensity) {
          intensity_decrease();
        }
        break;

      case 0x02: // Responsive Intensity
        if (responsive_mode) {
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
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(0, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(0, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x05:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(85, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(85, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x06:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(170, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(170, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x07: //Rainbow state
        active_state = RAINBOW;
        cycle_task.disable();
        fill_rainbow(leds_buffer, LED_COUNT, 0, 255 / LED_COUNT);
        if (!responsive_mode) {
          cpyBuffer(leds_buffer, leds);
          FastLED.show();
        }

        break;

      case 0x08:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(17, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(17, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x09:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(102, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(102, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0A:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(187, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(187, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0B: // Static state
        active_state = STATIC;
        cycle_task.disable();
        set_static_color(DEFAULT_STATIC_COLOR);
        break;

      case 0x0C:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(34, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(34, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0D:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(119, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(119, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0E:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(204, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(204, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0F: //Gradient state
        active_state = GRADIENT;
        cycle_task.disable();
        FastLED.clear();
        FastLED.show();
        break;

      case 0x10:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(51, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(51, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x11:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(136, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(136, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x12:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(221, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(221, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x13: //Cycle state
        active_state = CYCLE;
        cycle_task.enable();
        break;

      case 0x14:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(68, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(68, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x15:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(153, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(153, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x16:
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(238, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(238, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x17: // Responsive Mode
        responsive_mode = !responsive_mode;
        if (responsive_mode) {
          FastLED.clear();
          FastLED.show();
          midi_task.enable();
        } else {
          cpyBuffer(leds_buffer, leds);
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
