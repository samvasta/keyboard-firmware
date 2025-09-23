#pragma once

#include "common.h"

/* key matrix position */
typedef struct {
	int row;
	int col;
} keypos_t;

/* key event */
typedef struct {
	keypos_t key;
	bool     pressed;
} keyevent_t;

void init_matrix();

void process_matrix(matrix_state_t next_state);

void on_key_pressed(keycode_t code, keypos_t pos);
void on_key_released(keypos_t pos);

keycode_t handle_press(keycode_t code, uint8_t position, uint8_t layer);
void handle_release(keycode_t code, uint8_t position, uint8_t layer);

void add_modifier(keycode_t mod);
void remove_modifier(keycode_t mod);


void active_keys_to_hid_codes();
keycode_t get_hid_code(keycode_t code);