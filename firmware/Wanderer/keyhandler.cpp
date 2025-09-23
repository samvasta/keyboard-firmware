#include "common.h"
#include "matrix.h"
#include "keyhandler.h"
#include "keymap.h"
#include "settings.h"
#include "usb.h"
#include "debug.h"

static keycode_t active_keys[NUM_COLS * NUM_ROWS];
static keycode_t hid_codes[NUM_COLS * NUM_ROWS] = {0};

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

inline static uint32_t get_actual_index(uint8_t row, uint8_t col)
{
	uint8_t actual_col = col;
	if (row == NUM_ROWS - 1)
	{
		actual_col = actual_col - 2;
	}
	return NUM_COLS * row + actual_col;
}

inline static keycode_t get_keycode(uint8_t row, uint8_t col)
{
	uint32_t idx = get_actual_index(row, col);
	keycode_t code = layers[layer][idx];
	uint8_t l = layer;
	while(code == KEY_TRANS) {
		if(l > 0) {
			code = layers[l--][idx];
		} else {
			code = alphas[alpha_layer][idx];
		}
	}
	return code;
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

	for (int i = 0; i < NUM_ROWS * NUM_COLS; i++)
	{
		active_keys[i] = 0;
	}
	prev_state = 0;
	debouncing_state = 0;
	layer = 0;
	alpha_layer = eeprom_read(EEPROM_ADDR_BASE_LAYER, ALPHA_QWERTY) % NUM_ALPHA_LAYERS;
	os_mode = eeprom_read(EEPROM_ADDR_OS_MODE, OS_WIN_LIN);
	prev_layer = layer;
	debouncing = false;
}

void print_matrix(matrix_state_t matrix)
{
	D_println("Matrix state:");
	for (int r = 0; r < NUM_ROWS; r++)
	{
		for (int c = 0; c < NUM_COLS; c++)
		{
			D_print(is_pressed(matrix, r, c) ? "X" : ".");
			if (c == (NUM_COLS / 2 - 1))
			{
				D_print("   ");
			}
		}
		D_println();
	}
	D_printf("(%016llx)\n", matrix);
}

void process_matrix(matrix_state_t next_state)
{
	#ifdef DEBUG
	if (next_state != prev_state)
	{
		print_matrix(next_state);
	}
	#endif

	for (int r = 0; r < NUM_ROWS; r++)
	{
		for (int c = 0; c < NUM_COLS; c++)
		{
			bool pressed = is_pressed(next_state, r, c);
			bool prev_pressed = is_pressed(prev_state, r, c);

			bool changed = pressed != prev_pressed;

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

	active_keys_to_hid_codes();
	process_hid(hid_codes, modifiers);
	
	prev_state = next_state;
	prev_layer = layer;
}

void on_key_pressed(keycode_t code, keypos_t pos)
{
	int index = get_actual_index(pos.row, pos.col);
	keycode_t active = active_keys[index];

	if (active && active != code)
	{
		on_key_released(pos);
	}

	active_keys[index] = handle_press(code, index, layer);
}

void on_key_released(keypos_t pos)
{
	int index = get_actual_index(pos.row, pos.col);
	keycode_t active = active_keys[index];
	if (active == 0)
	{
		// Do nothing
	}
	else
	{
		handle_release(active, index, layer);
		prev_state &= ~((matrix_state_t)1 << index);
	}
	active_keys[index] = 0;
}

void add_layer(uint8_t l)
{
	uint8_t initial_layer = layer;
	layer = (layer | l) & 0xF;
	D_printf("Adding layer %d + %d = %d\n", initial_layer, l, layer);
}
void remove_layer(uint8_t l)
{
	uint8_t initial_layer = layer;
	layer = (layer & ~l) & 0xF;
	D_printf("Removing layer %d + %d = %d\n", initial_layer, l, layer);
}

void set_os_mode(enum os_mode mode)
{
	os_mode = mode;
	eeprom_write(EEPROM_ADDR_OS_MODE, mode);
}

keycode_t handle_press(keycode_t code, uint8_t position, uint8_t layer)
{
	D_printf("Handle press: code=%04x pos=%d layer=%d\n", code, position, layer);
	// Reserved for custom codes
	if ((code & 0xff00) == KEY_CODE_LAYER_RANGE_START)
	{
		D_println("Add layer");
		uint8_t l = code & 0x00ff;
		add_layer(l);
	}
	else if ((code & 0xff00) == KEY_CODE_ALPHA_RANGE_START)
	{
		D_println("Alpha layer");
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
				return handle_press(layers[layer - 1][position], position, layer - 1);
			}
			else
			{
				// reach into alpha layers
				return handle_press(alphas[alpha_layer][position], position, 0);
			}
			break;
		case CTRL_CMD:
			add_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			break;
		case CTRL_ALT:
			add_modifier(os_mode == OS_OSX ? LALT : LCTL);
			break;
		case ALT_CMD:
			add_modifier(os_mode == OS_OSX ? LGUI : LALT);
			break;
		case SHIFT_ALT:
			add_modifier(os_mode == OS_OSX ? LALT : LSFT);
			break;
		case COPY:
			add_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			return COPY;
		case PASTE:
			add_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			return PASTE;
		case CUT:
			add_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			return CUT;
		case UNDO:
			add_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			return UNDO;
		case REDO:
			if (os_mode == OS_OSX)
			{
				add_modifier(LGUI);
				add_modifier(LSFT);
				return REDO;
			}
			else
			{
				add_modifier(LCTL);
				return REDO;
			}
			break;
		case SELECT_ALL:
			add_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			return SELECT_ALL;
		case OSX:
			set_os_mode(OS_OSX);
			break;
		case LINUX:
			set_os_mode(OS_WIN_LIN);
			break;
		case OS_TOG:
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
	else if ((code & 0xff00) == KEY_CODE_SHIFTED_CHAR)
	{
		// Shifted character
		add_modifier(LSFT);
	}
	else
	{
		if (is_hid_modifier(code))
		{
			add_modifier(code);
		}
		// Must be a HID keycode
	}

	return code;
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

		case CTRL_CMD:
			remove_modifier(os_mode == OS_OSX ? LGUI : LCTL);
			break;
		case CTRL_ALT:
			remove_modifier(os_mode == OS_OSX ? LALT : LCTL);
			break;
		case ALT_CMD:
			remove_modifier(os_mode == OS_OSX ? LGUI : LALT);
			break;
		case SHIFT_ALT:
			remove_modifier(os_mode == OS_OSX ? LALT : LSFT);
			break;
		case COPY:
			active_keys[position] = 0;
		case PASTE:
			active_keys[position] = 0;
		case CUT:
			active_keys[position] = 0;
		case UNDO:
			active_keys[position] = 0;
		case REDO:
			active_keys[position] = 0;
		case SELECT_ALL:
			active_keys[position] = 0;
		case OSX:
		case LINUX:
		case OS_TOG:
			// nothing to do
			return;
		default:
			// Unknown custom keycode
			break;
		}
	}
	else if ((code & 0xff00) == KEY_CODE_SHIFTED_CHAR)
	{
		// nothing to do
		remove_modifier(LSFT);
	}
	else
	{
		if (is_hid_modifier(code))
		{
			remove_modifier(code);
		}
		// Must be a HID keycode

		active_keys[position] = 0;
	}
}

void add_modifier(keycode_t mod)
{
	modifiers |= (0x1 << (mod & 0xF));
}
void remove_modifier(keycode_t mod)
{
	modifiers &= ~(0x1 << (mod & 0xF));
}

void active_keys_to_hid_codes()
{
	for (int i = 0; i < NUM_COLS * NUM_ROWS; i++)
	{
		if (!is_hid_code(active_keys[i]))
		{
			hid_codes[i] = get_hid_code(active_keys[i]);
		}
		else
		{
			hid_codes[i] = active_keys[i];
		}
	}
}

keycode_t get_hid_code(keycode_t code)
{
	if ((code & 0xff00) == KEY_CODE_SHIFTED_CHAR)
	{
		// Shifted character
		return code & 0x00ff;
	}
	else if ((code & 0xff00) == CUSTOM_KEY_CODE_RANGE_START)
	{
		switch (code)
		{
		case COPY:
			return KEY_C;
		case PASTE:
			return KEY_V;
		case CUT:
			return KEY_X;
		case UNDO:
			return KEY_Z;
		case REDO:
			if (os_mode == OS_OSX)
			{
				return KEY_Z;
			}
			else
			{
				return KEY_Y;
			}
		case SELECT_ALL:
			return KEY_A;
		default:
			return 0;
		}
	}
	else if ((code & 0xff00) == KEY_CODE_LAYER_RANGE_START)
	{
		// Layer key, no HID code
		return 0;
	}
	else if ((code & 0xff00) == KEY_CODE_ALPHA_RANGE_START)
	{
		// Alpha layer key, no HID code
		return 0;
	}
	else
	{
		return code;
	}
}