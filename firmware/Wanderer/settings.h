#pragma once
#include "common.h"

#define EEPROM_SIZE 512
#define EEPROM_START 0

enum settings_addr {
  EEPROM_ADDR_BASE_LAYER = EEPROM_START,
  EEPROM_ADDR_OS_MODE,
};

enum os_mode {
  OS_WIN_LIN = 0,
  OS_OSX,
};


void eeprom_init();
uint8_t eeprom_read(settings_addr addr, uint8_t value);
void eeprom_write(settings_addr addr, uint8_t value);


