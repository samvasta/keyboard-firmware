#include "matrix.h"
#include "usb.h"
#include "debug.h"

void setup() {
  D_SerialBegin(9600);

  setup_usb();
  setup_matrix();
}

static uint32_t last_loop_start;
static uint32_t last_loop_time;

void loop() {
  last_loop_start = micros();
  scan();
  last_loop_time = micros() - last_loop_start;
  if(last_loop_time < 1000) {
    delayMicroseconds(1000 - last_loop_time);
  }
}
