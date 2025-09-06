#include "matrix.h"
#include "keyhandler.h"
#include <Wire.h>
#include <MCP23017.h>

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

	Serial1.begin(9600);
}

void scan()
{

	// Tell follower to scan. In the meantime we'll process the left half of the matrix
	Serial1.write('S');
	Serial1.flush();

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

	// Right hand
	uint8_t tries = 0;

	char msg_instruction[1] = {0};
	while(!Serial1.available() && tries < 100) {
		tries++;
		delayMicroseconds(50);
	}
	if(tries >= 100) {
		process_matrix(state);
		return;
	}

	Serial1.readBytesUntil(0xff, msg_instruction, 1); // wait for the start byte


	for (int i = 0; i < NUM_ROWS; i++)
	{
		while(!Serial1.available()) {
			delayMicroseconds(50);
		}
		uint8_t row_state = Serial1.read();
		for (int c = 0; c < NUM_COLS / 2; c++)
		{
			// Column is pressed if bit c is low
			auto isPressed = (row_state >> c) & 0x1;
			state = set_key(state, i, c + NUM_COLS / 2, isPressed);
		}
	}

	while(!Serial1.available() && tries < 100) {
		tries++;
		delayMicroseconds(50);
	}
	Serial1.readBytesUntil(0xee, msg_instruction, 1); // wait for the end byte

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