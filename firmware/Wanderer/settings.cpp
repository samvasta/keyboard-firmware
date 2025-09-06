#include "settings.h"

#include <EEPROM.h>

void eeprom_init()
{
  EEPROM.begin(EEPROM_SIZE);
}

uint8_t eeprom_read(settings_addr addr, uint8_t value)
{
  if (addr > EEPROM_SIZE)
    return value;

  return (uint8_t)EEPROM.read(addr);
}

void eeprom_write(settings_addr addr, uint8_t value)
{
  if (addr > EEPROM_SIZE)
    return;
    
  EEPROM.write(addr, value);

  EEPROM.commit();
}