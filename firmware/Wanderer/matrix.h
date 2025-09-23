#pragma once

#include "keys.h"
#include "common.h"

const uint8_t NUM_LAYER_BUTTONS = 4;

const uint8_t NUM_LAYERS = 2 << (NUM_LAYER_BUTTONS - 1);

const uint8_t NUM_ROWS = 4;
const uint8_t NUM_COLS = 12;

const uint8_t NUM_KEYS = NUM_COLS * NUM_ROWS;

const uint8_t LH_COL_PINS[] = { 14, 15, 26, 27, 28, 29 };
const uint8_t LH_ROW_PINS[] = { 13, 12, 11, 10 };

// #define RH_COL_PINS { A0, A1, A2, A3, A4, A5 }
// #define RH_ROW_PINS { B0, B1, B2, B3}

typedef uint64_t matrix_state_t;

#define keystate(matrix,row,col) (matrix & (1 << (row * NUM_COLS + col)))

#define MAKE_LAYER( \
  L00, L01, L02, L03, L04, L05,                 R05, R04, R03, R02, R01, R00, \
  L10, L11, L12, L13, L14, L15,                 R15, R14, R13, R12, R11, R10, \
  L20, L21, L22, L23, L24, L25,                 R25, R24, R23, R22, R21, R20, \
            L32, L33, L34, L35,                 R35, R34, R33, R32 \
) {\
  L00, L01, L02, L03, L04, L05,                 R05, R04, R03, R02, R01, R00, \
  L10, L11, L12, L13, L14, L15,                 R15, R14, R13, R12, R11, R10, \
  L20, L21, L22, L23, L24, L25,                 R25, R24, R23, R22, R21, R20, \
            L32, L33, L34, L35,                 R35, R34, R33, R32 \
}

#define MAKE_ALPHA_LAYER( \
  L01, L02, L03, L04, L05,                 R05, R04, R03, R02, R01, \
  L11, L12, L13, L14, L15,                 R15, R14, R13, R12, R11, \
  L21, L22, L23, L24, L25,                 R25, R24, R23, R22, R21 \
) {\
  ESC,      L01,  L02,  L03, L04,  L05,      R05,  R04,  R03,  R02,  R01, BSPC, \
  TAB,      L11,  L12,  L13, L14,  L15,      R15,  R14,  R13,  R12,  R11, ENT, \
  CTRL_CMD, L21,  L22,  L23, L24,  L25,      R25,  R24,  R23,  R22,  R21, RSFT, \
              L(2), SPC, L(1), L(4),     L(8), RSFT, L(2), L(1) \
}

matrix_state_t set_key(matrix_state_t matrix, uint8_t row, uint8_t col, bool pressed);

void setup_matrix();
void scan();