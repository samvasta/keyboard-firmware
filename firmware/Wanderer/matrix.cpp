#include "matrix.h"
#include "keyhandler.h"
#include <Wire.h>
#include <MCP23017.h>

#define MCP23017_ADDR 0x20
MCP23017 mcp = MCP23017(MCP23017_ADDR);

#define MCP_PORT_ROW MCP23017Port::A
#define MCP_PORT_COL MCP23017Port::B

void setup_matrix() {
  // Right Hand
  Wire.begin();

  mcp.init();
  mcp.portMode(MCP_PORT_ROW, 0b11111111); // Port A (Rows) is input
  mcp.portMode(MCP_PORT_COL, 0);   // Port B (Columns) is output

  mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A 
  mcp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B

  // Default row state is on
  mcp.writeRegister(MCP23017Register::IPOL_A, 0xFF);

  //TODO
}

void scan(){
  matrix_state_t state = 0;
  // Left hand
  for(int r = 0; r < NUM_ROWS; r++) {
    for(int c = 0; c < NUM_COLS/2; c++) {
      //TODO
    }
  }

  // Right hand

  for(int r = 0; r < NUM_ROWS; r++) {
    mcp.writePort(MCP_PORT_ROW, ~((uint8_t)1 << r));
    uint8_t col_state = mcp.readPort(MCP23017Port::B);
    for(int c = 0; c < NUM_COLS/2; c++) {
      bool isPressed = ((col_state >> c) & 0x1);
      state = set_key(state, r, c, isPressed);
    }
  }

  process_matrix(state);
}


matrix_state_t set_key(matrix_state_t matrix, uint8_t row, uint8_t col, bool pressed) {
  matrix_state_t next = matrix;
  uint16_t index = (row * NUM_COLS + col);
  if(pressed) {
    return (matrix | (1 << index));
  } else {
    return matrix & ~((matrix_state_t)1 << index);
  }
}