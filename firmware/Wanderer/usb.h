#pragma once

#include "common.h"
#include <Adafruit_TinyUSB.h>

// the setup function runs once when you press reset or power the board
void setup_usb();

void process_hid(keycode_t active_keys[], uint8_t modifiers);

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);

void usb_write_str(const char *str, uint16_t len);