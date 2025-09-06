#include "matrix.h"
#include "usb.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Wanderer Keyboard");

  setup_usb();
  setup_matrix();
}

void loop() {
  scan();
}
