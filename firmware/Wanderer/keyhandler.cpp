#include "common.h"
#include "matrix.h"
#include "keyhandler.h"
#include "keymap.h"
#include "settings.h"
#include "usb.h"

static keycode_t active_keys[NUM_COLS * NUM_ROWS];

static matrix_state_t prev_state;
static matrix_state_t debouncing_state;

static uint16_t debouncing_time;
static bool debouncing = false;

volatile uint8_t prev_layer;
volatile uint8_t layer;
keyevent_t pressed_state[NUM_ROWS * NUM_COLS];
volatile uint8_t modifiers = 0;

volatile uint8_t alpha_layer = ALPHA_QWERTY;
volatile uint8_t os_mode = OS_WIN_LIN;

static inline bool is_pressed(matrix_state_t matrix, uint8_t row, uint8_t col)
{
	return matrix & ((matrix_state_t)1U << (row * NUM_COLS + col));
}

inline static keycode_t get_keycode(uint8_t row, uint8_t col)
{
	return layers[layer][NUM_COLS * row + col];
}

void init_matrix()
{
	for (uint8_t r = 0; r < NUM_ROWS; r++)
	{
		for (uint8_t c = 0; c < NUM_COLS; c++)
		{
			pressed_state[r * NUM_COLS + c] =
					{
							.key = {r, c},
							.pressed = 0};
		}
	}

	for (int i = 0; i < 6; i++)
	{
		active_keys[i] = 0;
	}
	prev_state = 0;
	debouncing_state = 0;
	layer = eeprom_read(EEPROM_ADDR_BASE_LAYER, ALPHA_QWERTY);
	alpha_layer = layer % NUM_ALPHA_LAYERS;
	os_mode = eeprom_read(EEPROM_ADDR_OS_MODE, OS_WIN_LIN);
	prev_layer = layer;
	debouncing = false;
}

void print_matrix(matrix_state_t matrix)
{
	Serial.println("Matrix state:");
	for (int r = 0; r < NUM_ROWS; r++)
	{
		for (int c = 0; c < NUM_COLS; c++)
		{
			Serial.print(is_pressed(matrix, r, c) ? "X" : ".");
			if (c == (NUM_COLS / 2 - 1))
			{
				Serial.print("   ");
			}
		}
		Serial.println();
	}
	Serial.printf("(%016llx)\n", matrix);
}

void process_matrix(matrix_state_t next_state)
{
	if (next_state != prev_state)
	{
		print_matrix(next_state);
	}

	for (int r = 0; r < NUM_ROWS; r++)
	{
		for (int c = 0; c < NUM_COLS; c++)
		{
			bool pressed = is_pressed(next_state, r, c);
			bool prev_pressed = is_pressed(prev_state, r, c);

			bool changed = pressed ^ prev_pressed;

			if (changed)
			{
				keypos_t pos = {row : r, col : c};
				if (pressed)
				{
					keycode_t code = get_keycode(r, c);
					on_key_pressed(code, pos);
				}
				else
				{
					on_key_released(pos);
				}
			}
		}
	}

	prev_state = next_state;

	process_hid(active_keys, modifiers);
	prev_layer = layer;
}

void on_key_pressed(keycode_t code, keypos_t pos)
{
	int index = pos.row * NUM_COLS + pos.col;
	keycode_t active = active_keys[index];

	if (active && active != code)
	{
		on_key_released(pos);
	}

	handle_press(code, pos.row * NUM_COLS + pos.col, layer);

	prev_state |= ((matrix_state_t)1 << index);
}

void on_key_released(keypos_t pos)
{
	int index = pos.row * NUM_COLS + pos.col;
	keycode_t active = active_keys[index];
	if (active == 0)
	{
		// Do nothing
	}
	else
	{
		handle_release(active, pos.row * NUM_COLS + pos.col, layer);
		prev_state &= ~((matrix_state_t)1 << index);
	}
}

void add_layer(uint8_t l)
{
	layer = (layer | l);
}
void remove_layer(uint8_t l)
{
	layer = (layer & ~l);
}

void set_os_mode(enum os_mode mode)
{
	os_mode = mode;
	eeprom_write(EEPROM_ADDR_OS_MODE, mode);
}

void handle_press(keycode_t code, uint8_t position, uint8_t layer)
{
	Serial.printf("Handle press: code=%04x pos=%d layer=%d\n", code, position, layer);
	// Reserved for custom codes
	if ((code & 0xff00) == KEY_CODE_LAYER_RANGE_START)
	{
		Serial.println("Add layer");
		uint8_t l = code & 0x00ff;
		add_layer(l);
	}
	else if ((code & 0xff00) == KEY_CODE_ALPHA_RANGE_START)
	{
		Serial.println("Alpha layer");
		uint8_t l = code & 0x00ff;
		if (l < NUM_ALPHA_LAYERS)
		{
			alpha_layer = l % NUM_ALPHA_LAYERS;
			eeprom_write(EEPROM_ADDR_BASE_LAYER, l);
		}
	}
	else if ((code & 0xff00) == CUSTOM_KEY_CODE_RANGE_START)
	{
		switch (code)
		{
		case KEY_NO:
			// opaque, no-op
			break;
		case KEY_TRANS:
			// Look down a layer
			if (layer > 0)
			{
				handle_press(layers[layer - 1][position], position, layer - 1);
			}
			else
			{
				// reach into alpha layers
				handle_press(alphas[alpha_layer][position], position, 0);
			}
			break;
		case KEY_COPY:
			modifiers = os_mode == OS_OSX ? KEY_LGUI : KEY_LCTL;
			active_keys[position] = KEY_C;
		case KEY_PASTE:
			modifiers = os_mode == OS_OSX ? KEY_LGUI : KEY_LCTL;
			active_keys[position] = KEY_V;
			break;
		case KEY_CUT:
			modifiers = os_mode == OS_OSX ? KEY_LGUI : KEY_LCTL;
			active_keys[position] = KEY_X;
			break;
		case KEY_UNDO:
			modifiers = os_mode == OS_OSX ? KEY_LGUI : KEY_LCTL;
			active_keys[position] = KEY_Z;
			break;
		case KEY_REDO:
			modifiers = os_mode == OS_OSX ? KEY_LGUI : KEY_LCTL;
			active_keys[position] = KEY_Y;
			break;
		case KEY_SELECT_ALL:
			modifiers = os_mode == OS_OSX ? KEY_LGUI : KEY_LCTL;
			active_keys[position] = KEY_A;
			break;

		case KEY_OSX:
			set_os_mode(OS_OSX);
			break;
		case KEY_LINUX:
			set_os_mode(OS_WIN_LIN);
			break;
		case KEY_OS_TOGGLE:
			if (os_mode == OS_OSX)
			{
				set_os_mode(OS_WIN_LIN);
			}
			else
			{
				set_os_mode(OS_OSX);
			}
			break;
		default:
			// Unknown custom keycode
			break;
		}
	}
	else
	{
		// Must be a HID keycode
		active_keys[position] = code;
	}
}

void handle_release(keycode_t code, uint8_t position, uint8_t layer)
{
	// Reserved for custom codes
	if ((code & 0xff00) == KEY_CODE_LAYER_RANGE_START)
	{
		uint8_t l = code & 0x00ff;
		remove_layer(l);
	}
	else if ((code & 0xff00) == KEY_CODE_ALPHA_RANGE_START)
	{
		// nothing to do
		return;
	}
	else if ((code & 0xff00) == CUSTOM_KEY_CODE_RANGE_START)
	{
		switch (code)
		{
		case KEY_NO:
			// opaque, no-op
			return;
		case KEY_TRANS:
			// Look down a layer
			if (layer > 0)
			{
				handle_release(layers[layer - 1][position], position, layer - 1);
			}
			else
			{
				// reach into alpha layers
				handle_release(alphas[alpha_layer][position], position, 0);
			}
			return;
		case KEY_COPY:
		case KEY_PASTE:
		case KEY_CUT:
		case KEY_UNDO:
		case KEY_REDO:
		case KEY_SELECT_ALL:
		case KEY_OSX:
		case KEY_LINUX:
		case KEY_OS_TOGGLE:
			// nothing to do
			return;
		default:
			// Unknown custom keycode
			break;
		}
	}
	else
	{
		// Must be a HID keycode
		active_keys[position] = 0;
	}
}