#include "midi.h"
#include "led.h"

uint8_t led_mapping[88] = {
    // Mapping: index|value 'piano key'|'led'
    0,  // A  0
    1,  // A# 0
    2,  // B  0
    3,  // C  1
    4,  // C# 1
    4,  // D  1
    5,  // D# 1
    6,  // E  1
    7,  // F  1
    8,  // F# 1
    9,  // G  1
    9,  // G# 1
    10, // A  1
    11, // A# 1
    12, // B  1
    13, // C  2
    14, // C# 2
    14, // D  2
    15, // D# 2
    16, // E  2
    17, // F  2
    18, // F# 2
    19, // G  2
    19, // G# 2
    20, // A  2
    21, // A# 2
    22, // B  2
    23, // C  3
    24, // C# 3
    24, // D  3
    25, // D# 3
    26, // E  3
    27, // F  3
    28, // F# 3
    28, // G  3
    29, // G# 3
    30, // A  3
    31, // A# 3
    32, // B  3
    33, // C  4
    33, // C# 4
    34, // D  4
    35, // D# 4
    36, // E  4
    37, // F  4
    38, // F# 4
    38, // G  4
    39, // G# 4
    40, // A  4
    41, // A# 4
    42, // B  4
    42, // C  5
    43, // C# 5
    44, // D  5
    45, // D# 5
    46, // E  5
    47, // F  5
    48, // F# 5
    48, // G  5
    49, // G# 5
    50, // A  5
    51, // A# 5
    51, // B  5
    52, // C  6
    53, // C# 6
    54, // D  6
    55, // D# 6
    56, // E  6
    57, // F  6
    57, // F# 6
    58, // G  6
    59, // G# 6
    60, // A  6
    61, // A# 6
    61, // B  6
    62, // C  7
    63, // C# 7
    64, // D  7
    65, // D# 7
    65, // E  7
    66, // F  7
    67, // F# 7
    68, // G  7
    69, // G# 7
    70, // A  7
    71, // A# 7
    72, // B  7
    73, // C  8
};

void midi_setup()
{
  if (Usb.Init() == -1)
  {
    while (1)
      ; // halt
  }
}

void MIDI_noteOn(uint8_t note, uint16_t velocity)
{
  note -= MIDI_INDEX_OFFSET;

  uint8_t led = led_mapping[note];
  switch (active_state)
  {
  case GRADIENT:
    leds[led] = get_gradient_value(LED_COUNT, led);
    break;
  case RAINBOW:
    leds[led] = get_rainbow_value(LED_COUNT, led);
    break;
  default:
    leds[led] = CHSV(active_hue, DEFAULT_SATURATION, DEFAULT_INTENSITY);
    break;
  }

  if (responsive_intensity)
  {
    leds[led].fadeLightBy(255 - (velocity * 7) / 4);
  }
}

void MIDI_noteOff(uint8_t note)
{
  note -= MIDI_INDEX_OFFSET;
  uint8_t led = led_mapping[note];
  leds[led] = BLANK_LED;
}

void MIDI_poll()
{
  Usb.Task();

  if (!Midi)
  {
    return;
  }

  uint8_t recvBuf[3]; //[0]: MIDI Command | [1]: MIDI Note | [2]: MIDI Velocity
  uint8_t size;
  static bool hasUpdated = false;

  do
  {
    size = Midi.RecvData(recvBuf);
    if (size > 0)
    {
      // MIDI Output
      switch (recvBuf[0])
      {
      case 144: // Note on
        MIDI_noteOn(recvBuf[1], recvBuf[2]);
        hasUpdated = true;
        break;

      case 128: // Note off
        MIDI_noteOff(recvBuf[1]);
        hasUpdated = true;
        break;

      default:
        break;
      }
    }
  } while (size > 1);

  if (hasUpdated)
  {
    FastLED.show();
    hasUpdated = false;
  }
}
