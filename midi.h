//USB Midi
#ifndef MIDI_H
#define MIDI_H

#include <usbh_midi.h>
#include <usbhub.h>
#define MIDI_INDEX_OFFSET 21 //Number of first key

USB Usb;
USBHub Hub(&Usb);
USBH_MIDI  Midi(&Usb);

void midi_setup(void);
void MIDI_noteOn(uint8_t note, uint16_t velocity);
void MIDI_poll(void);
#endif
