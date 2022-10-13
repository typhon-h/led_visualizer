#ifndef IR_H
#define IR_H
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 2

void ir_setup(void);
void ir_poll(void);
void ir_run_command(int command);
#endif
