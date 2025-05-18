#include "common.h"
#include "matrix.h"
#include "keyhandler.h"

static keycode_t active_keys[NUM_KEYS];

static matrix_state_t prev_state;
static matrix_state_t debouncing_state;

static uint16_t debouncing_time;
static bool debouncing = false;

void init_matrix() {
  for(int i = 0; i < NUM_KEYS; i++) {
    active_keys[i] = 0;
  }
  prev_state = 0;
  debouncing_state = 0;
}

void process_matrix(matrix_state_t next_state) {

}

void on_key_pressed(keycode_t code, keypos_t pos) {
  handle_press(code);

  int index = pos.row * NUM_COLS + pos.col;
  active_keys[index] = code;
  prev_state |= ((matrix_state_t)1 << index);
}

void on_key_released(keypos_t pos) {
  int index = pos.row * NUM_COLS + pos.col;
  keycode_t active = active_keys[index];
  if (active == 0) {
    // Do nothing
  } else {
    handle_release(active);
    prev_state &= ~((matrix_state_t)1 << index);
  }

  active_keys[index] = 0;
}

void handle_press(keycode_t code) {
  //TODO
}

void handle_release(keycode_t code) {
  //TODO
}