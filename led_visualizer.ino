#include "led.h"
#include "midi.h"
#include "ir.h"

// Task Scheduler
#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>

#define IR_TASK_FREQUENCY 400
#define CYCLE_TASK_FREQUENCY 250
#define MIDI_TASK_FREQUENCY 50

Task ir_task(IR_TASK_FREQUENCY, TASK_FOREVER, &ir_poll);
Task cycle_task(CYCLE_TASK_FREQUENCY, TASK_FOREVER, &color_cycle);
Task midi_task(MIDI_TASK_FREQUENCY, TASK_FOREVER, &MIDI_poll);
Scheduler schedule;

void setup()
{
  Serial.begin(115200);
  ir_setup();
  led_setup();
  midi_setup();

  schedule.init();
  schedule.addTask(ir_task);
  schedule.addTask(cycle_task);
  schedule.addTask(midi_task);

  ir_task.enable();
  cycle_task.disable();
  midi_task.disable();

  // ir_run_command(0x17); // Static default
  ir_run_command(0x13);
}

void loop()
{
  schedule.execute();
}
