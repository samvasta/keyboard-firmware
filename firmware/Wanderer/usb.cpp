#include <Adafruit_TinyUSB.h>
#include "usb.h"
#include "matrix.h"
#include "keys.h"
#include "debug.h"

// Report ID
enum
{
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
};

// HID report descriptor using TinyUSB's template
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
    // TUD_HID_REPORT_DESC_MOUSE   (HID_REPORT_ID(RID_MOUSE)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL))};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid;

// the setup function runs once when you press reset or power the board
void setup_usb()
{
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized())
  {
    TinyUSBDevice.begin(0);
  }

  // Setup HID
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("Keyboard");

  TinyUSBDevice.setID(0x5361, 0x6D56); // Sa mV
  TinyUSBDevice.setManufacturerDescriptor("Sam Vasta");
  TinyUSBDevice.setProductDescriptor("Wanderer Keyboard");
  ;

  usb_hid.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted())
  {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
  D_println("Adafruit TinyUSB HID Composite example");
}

void usb_write_str(const char *str)
{
  usb_hid.keyboardRelease(RID_KEYBOARD);
  int i = 0;
  while (str[i])
  {
    usb_hid.keyboardPress(RID_KEYBOARD, str[i]);
    usb_hid.keyboardRelease(RID_KEYBOARD);
    i++;
  }
}

// static keycode_t prior_active_keys[NUM_KEYS] = {0};

void process_hid(keycode_t active_keys[], uint8_t modifiers)
{
  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted())
  {
    return;
  }

  uint8_t keycode[6] = {0};
  uint16_t consumer_code = 0;

  uint8_t pressed_count = 0;
  for (int i = 0; i < NUM_KEYS; i++)
  {
    if (active_keys[i])
    {
      if (is_consumer_control_key(active_keys[i]))
      {
        consumer_code = get_consumer_control_keycode(active_keys[i]);
        break;
      }
      else if (is_hid_code(active_keys[i]) && pressed_count < 6)
      {
        keycode[pressed_count++] = active_keys[i];
      }
      else {
        D_printf("HID: Ignoring keycode %x (pos: %d)\n", active_keys[i], i);
      }
    }
  }

  // used to avoid send multiple consecutive zero report for keyboard
  static bool keyPressedPreviously = false;
  static bool consumerKeyPressedPreviously = false;

  if (TinyUSBDevice.suspended() && (keycode[0] || consumer_code))
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  // skip if hid is not ready e.g still transferring previous report
  if (!usb_hid.ready())
  {
    D_println("usb not ready");
    return;
  }

  // Process system control
  if (consumer_code)
  {
    consumerKeyPressedPreviously = true;
    D_println("usb consumer report");
    usb_hid.sendReport16(RID_CONSUMER_CONTROL, consumer_code);
  }
  else if (consumerKeyPressedPreviously)
  {
    consumerKeyPressedPreviously = false;
    usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
 }

  // Process keyboard
  if (keycode[0] || modifiers)
  {
    // Send report if there is key pressed
    keyPressedPreviously = true;
    // D_println("usb key press");
    usb_hid.keyboardReport(RID_KEYBOARD, modifiers, keycode);
  }
  else if (keyPressedPreviously)
  {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    keyPressedPreviously = false;
    usb_hid.keyboardRelease(RID_KEYBOARD);
  }
}

void send_hid_immediately(const uint8_t keycode[6], uint8_t modifiers)
{
  uint8_t keycodes[6] = {0};
  memcpy(keycodes, keycode, 6);
  usb_hid.keyboardRelease(RID_KEYBOARD);
  usb_hid.keyboardReport(RID_KEYBOARD, modifiers, keycodes);
}
