#include "matrix.h"
#include "keyhandler.h"
#include "debug.h"

#define MAX_RIGHT_HAND_TRIES 100

void setup_matrix()
{
	for (int r = 0; r < NUM_ROWS; r++)
	{
		pinMode(LH_ROW_PINS[r], INPUT_PULLUP);
	}
	for (int c = 0; c < NUM_COLS / 2; c++)
	{
		digitalWrite(LH_COL_PINS[c], HIGH);
		pinMode(LH_COL_PINS[c], OUTPUT);
	}

	init_matrix();
	D_println("Init matrix done.");

	Serial1.begin(115200);
	// Wait for right hand to initialize
	delay(30);
	D_println("Init Serial1 done.");
}

void scan()
{
	// Tell follower to scan. In the meantime we'll process the left half of the matrix
	bool didPollSerial1 = false;
	if(Serial1.availableForWrite()) {
		Serial1.write('S');
		didPollSerial1 = true;
	}

	matrix_state_t state = 0;
	// Left hand
	for (int c = 0; c < NUM_COLS / 2; c++)
	{
		digitalWrite(LH_COL_PINS[c], LOW);
		// Delay for the pin to settle
		delayMicroseconds(30);
		// Read rows
		for (int r = 0; r < NUM_ROWS; r++)
		{
			state = set_key(state, r, c, !digitalRead(LH_ROW_PINS[r]));
		}
		digitalWrite(LH_COL_PINS[c], HIGH);
	}

	if(!didPollSerial1) {
		process_matrix(state);
		return;
	}

	// Right hand
	uint8_t tries = 0;
	while (!Serial1.available() && tries < MAX_RIGHT_HAND_TRIES)
	{
		tries++;
		delayMicroseconds(150);
	}
	if (tries >= MAX_RIGHT_HAND_TRIES)
	{
		process_matrix(state);
	}
	// We have data!

	uint8_t lastByte = 0;
	while (Serial1.available() && lastByte != 0xff && tries < MAX_RIGHT_HAND_TRIES)
	{
		lastByte = Serial1.read();
		tries++;
	}

	if (lastByte != 0xff || tries >= MAX_RIGHT_HAND_TRIES)
	{
		process_matrix(state);
		return;
	}

	for (int i = 0; i < NUM_ROWS; i++)
	{
		uint8_t row_state = Serial1.read();
		if (row_state == 0)
		{
			D_println("Row state was empty, expected 2 final bits to be 0b11");
			continue;
		}
		for (int c = 0; c < NUM_COLS / 2; c++)
		{
			// Column is pressed if bit c is high
			auto isPressed = (row_state >> c) & 0x1;
			state = set_key(state, i, NUM_COLS - c - 1, isPressed);
		}
	}

	while (lastByte != 0xee && Serial1.available() && tries < MAX_RIGHT_HAND_TRIES)
	{
		lastByte = Serial1.read();
		tries++;
	}

	process_matrix(state);
}

matrix_state_t set_key(matrix_state_t matrix, uint8_t row, uint8_t col, bool pressed)
{
	matrix_state_t next = matrix;
	uint16_t index = (row * NUM_COLS + col);
	if (pressed)
	{
		return (matrix | ((matrix_state_t)1 << index));
	}
	else
	{
		return matrix & ~((matrix_state_t)1 << index);
	}
}