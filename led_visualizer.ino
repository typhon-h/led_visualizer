//DEFINITIONS –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

//Task Scheduler
#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>
#define IR_TASK_FREQUENCY 150
#define CYCLE_TASK_FREQUENCY 150
#define RESPONSIVE_FADE_TASK_FREQUENCY 80
#define MIDI_TASK_FREQUENCY 80


//IR Remote
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 2


//FastLED
#include "FastLED.h"
#define LED_PIN 6
#define LED_COUNT 74
#define DELTA_BRIGHTNESS 5
#define DEFAULT_INTENSITY 255
#define DEFAULT_SATURATION 255
#define DEFAULT_STATIC_COLOR CHSV(0, DEFAULT_SATURATION, DEFAULT_INTENSITY)
#define BLANK_LED CHSV(0, 0, 0)
CRGB leds[LED_COUNT];
CRGB leds_buffer[LED_COUNT];


//USB Midi
#include <usbh_midi.h>
#include <usbhub.h>
USB Usb;
USBHub Hub(&Usb);
USBH_MIDI  Midi(&Usb);


typedef enum {
  STATIC = 1, //Solid color
  CYCLE, //Timed rainbow cycle
  GRADIENT, //Gradient from primary to secondary
  RAINBOW //Rainbow gradient
} COLOR_STATE_t;
static COLOR_STATE_t active_state = STATIC; //Default mode
static int intensity = DEFAULT_INTENSITY;


static bool power_on = true;
static bool responsive_mode = false;
static bool responsive_intensity = false;


#define MIDI_INDEX_OFFSET 21 //Number of first key
uint8_t led_mapping[88] = { //Pseudo dictionary 'piano key':'led'
   0, //A0
   1, //A#0
   2, //B0
   3, //C1
   3, //C#1
   4, //D1
   5, //D#1
   6, //E1
   7, //F1
   8, //F#1
   9, //G1
   9, //G#1
  10, //A1
  11, //A#1
  12, //B1
  13, //C2
  13, //C#2
  14, //D2
  15, //D#2
  16, //E2
  17, //F2
  18, //F#2
  18, //G2
  19, //G#2
  20, //A2
  21, //A#2
  22, //B2
  23, //C3
  23, //C#3
  24, //D3
  25, //D#3
  26, //E3
  27, //F3
  28, //F#3
  28, //G3
  29, //G#3
  30, //A3
  31, //A#3
  32, //B3
  33, //C4
  33, //C#4
  34, //D4
  35, //D#4
  36, //E4
  37, //F4
  37, //F#4
  38, //G4
  39, //G#4
  40, //A4
  41, //A#4
  42, //B4
  43, //C5
  43, //C#5
  44, //D5
  45, //D#5
  46, //E5
  47, //F5
  47, //F#5
  48, //G5
  49, //G#5
  50, //A5
  51, //A#5
  51, //B5
  52, //C6
  53, //C#6
  54, //D6
  55, //D#6
  56, //E6
  57, //F6
  57, //F#6
  58, //G6
  59, //G#6
  60, //A6
  61, //A#6
  61, //B6
  62, //C7
  63, //C#7
  64, //D7
  65, //D#7
  66, //E7
  67, //F7
  67, //F#7
  68, //G7
  69, //G#7
  70, //A7
  71, //A#7
  72, //B7
  73  //C8
};

//END OF DEFINITIONS –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––


// SCHEDULER TASKS –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void ir_task_callback() { //Poll IR receiver 
  if (IrReceiver.decode()) {
    ir_run_command(IrReceiver.decodedIRData.command);
    IrReceiver.resume();
  }
}

void cycle_task_callback() { //Increment strip color cycle
  static uint8_t hue = 0;
  fill_solid(leds_buffer, LED_COUNT, CHSV(hue++, DEFAULT_SATURATION, DEFAULT_INTENSITY));
  if (!responsive_mode) {
    cpyBuffer(leds_buffer, leds);
    FastLED.show();
  }
}

void responsive_fade_task_callback() { //Fade all LEDs
  fadeToBlackBy(leds, LED_COUNT, 50);
}

void midi_task_callback() { //Poll USB midi
        Usb.Task();
    if (Midi) {
      MIDI_poll();
    }
}

Task ir_task(IR_TASK_FREQUENCY, TASK_FOREVER, &ir_task_callback);
Task cycle_task(CYCLE_TASK_FREQUENCY, TASK_FOREVER, &cycle_task_callback);
Task responsive_fade_task(RESPONSIVE_FADE_TASK_FREQUENCY, TASK_FOREVER, &responsive_fade_task_callback);
Task midi_task(MIDI_TASK_FREQUENCY, TASK_FOREVER, &midi_task_callback);
Scheduler schedule;
//END OF SCHEDULER TASKS –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––


//HELPER FUNCTIONS  –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void cpyBuffer(CRGB* src, CRGB* dest) {
  for (int i = 0; i < LED_COUNT; i++) {
    dest[i] = src[i];
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
  if (intensity < DELTA_BRIGHTNESS) {
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
  }

  if (secondary == BLANK_LED && color != primary) {
    secondary = color;
  }

  if (primary != BLANK_LED && secondary != BLANK_LED) {
    fill_gradient(leds_buffer, 0, primary, LED_COUNT, secondary);
    if (!responsive_mode) {
      cpyBuffer(leds_buffer, leds);
      FastLED.show();
    }

    primary = secondary = BLANK_LED;
  }
}

void MIDI_noteOn(uint8_t note, uint16_t velocity) {
  note -= MIDI_INDEX_OFFSET;
  
  uint8_t led = led_mapping[note];
  leds[led] = leds_buffer[led];
  
//  static int prev = note;
//  if(prev != note){
//  Serial.println(note);
//  }
//  prev = note;
  
  if (responsive_intensity) {
    //      Serial.println(velocity);
    leds[led].fadeLightBy(255 - (velocity * 7) / 4);
  }

}

void MIDI_poll() {
  uint8_t recvBuf[ 3 ]; //[0]: MIDI Command | [1]: MIDI Note | [2]: MIDI Velocity
  uint8_t size;

  do {
    Serial.println(size);
    if ( (size = Midi.RecvData(recvBuf)) > 0 ) {
      //MIDI Output
        if (recvBuf[0] == 144) { //144 = Note on command
          MIDI_noteOn(recvBuf[1], recvBuf[2]);
        }
    }
  } while (size > 1);

  FastLED.show();

}


void ir_run_command(int command) {
  if (!power_on && command == 0x03) {
    power_on = true;
    FastLED.setBrightness(intensity);
    FastLED.show();
    if (active_state == CYCLE) {
      cycle_task.enable();
    }

  }
  else { // Only register other buttons if power on
    ////
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

      case 0x03:
        power_on = false;
        FastLED.setBrightness(0);
        FastLED.show();

        cycle_task.disable();
        break;

      case 0x04: // Red
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

      case 0x05: // Yellow
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(42, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(42, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x06: // Blue
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(158, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(158, DEFAULT_SATURATION, DEFAULT_INTENSITY));
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

      case 0x08: // Red-Orange
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(6, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(6, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x09: // Yellow-Green
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(53, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(53, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0A: // Blue-Purple
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(179, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(179, DEFAULT_SATURATION, DEFAULT_INTENSITY));
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

      case 0x0C: // Orange
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(11, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(11, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0D: // Green
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(84, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(84, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0E: // Purple
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(189, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(189, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x0F: //Gradient state
        active_state = GRADIENT;
        cycle_task.disable();
        FastLED.clear(); // Visual Placeholder
        FastLED.show();
        break;

      case 0x10: // Yellow-Orange
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(32, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(32, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x11: // Blue-Green
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(126, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(126, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x12: // Red-Purple
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(210, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(210, DEFAULT_SATURATION, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x13: //Cycle state
        active_state = CYCLE;
        cycle_task.enable();
        break;

      case 0x14: // Full White
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(0, 0, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(0, 0, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x15: // Warm White
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(97, 128, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(97, 128, DEFAULT_INTENSITY));
            break;
          default:
            break;
        }

        break;

      case 0x16: // Cool White
        switch (active_state) {
          case STATIC:
            set_static_color(CHSV(97, 64, DEFAULT_INTENSITY));
            break;
          case GRADIENT:
            set_gradient(CHSV(97, 64, DEFAULT_INTENSITY));
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
          responsive_fade_task.enable();
          midi_task.enable();
        } else {
          cpyBuffer(leds_buffer, leds);
          FastLED.show();
          responsive_fade_task.disable();
          midi_task.disable();
          responsive_intensity = false;
        }
        break;

      default:
        break;
    }
  }
  delay(IR_TASK_FREQUENCY);
}


void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(intensity);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();

  if (Usb.Init() == -1) {
    while (1); //halt
  }

  schedule.init();
  schedule.addTask(ir_task);
  schedule.addTask(cycle_task);
  schedule.addTask(responsive_fade_task);
  schedule.addTask(midi_task);
  ir_task.enable();
  cycle_task.disable();
  responsive_fade_task.disable();
  midi_task.disable();

  ir_run_command(0x0B); //Static default
}


void loop() {
  schedule.execute();
}
