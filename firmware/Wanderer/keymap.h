/* cspell:disable  */
#pragma once

#include "matrix.h"
#include "keys.h"
#include "common.h"

const keycode_t alphas[NUM_ALPHA_LAYERS][NUM_KEYS] = {
  // QWERTY
  MAKE_ALPHA_LAYER(
      KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,        KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
      KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,        KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMI,
      KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,        KEY_N, KEY_M, COMMA, DOT,   SLSH
  ),
  // QGMLWY
  MAKE_ALPHA_LAYER(
      KEY_Q, KEY_G, KEY_M, KEY_L, KEY_W,        KEY_B, KEY_Y, KEY_U, KEY_V, KEY_P,
      KEY_D, KEY_S, KEY_T, KEY_N, KEY_R,        KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMI,
      KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,        KEY_N, KEY_M, COMMA, DOT,   SLSH
  ),
  // Nordrasil
  MAKE_LAYER(
      ESC,      KEY_Q, KEY_Y, KEY_O, KEY_U, KEY_Z,        KEY_J, KEY_G, KEY_N, KEY_F, KEY_K, BSPC,
      TAB,      KEY_H, KEY_I, KEY_E, KEY_A, DOT,          KEY_P, KEY_D, KEY_R, KEY_S, KEY_L, ENT,
      CTRL_CMD, KEY_X, APOS,  SLSH,  COMMA, SCLN,         KEY_B, KEY_C, KEY_M, KEY_W, KEY_V, RSFT,
                       L(2),  SPC,   L(1),  L(4),         RSFT,  L(2),  KEY_T, L(1)
  ),
  // Nordrasil (mirrored)
  MAKE_LAYER(
      ESC,      KEY_K, KEY_F, KEY_N, KEY_G, KEY_J,        KEY_Z, KEY_U, KEY_O, KEY_Y, KEY_Q, BSPC,
      TAB,      KEY_L, KEY_S, KEY_R, KEY_D, KEY_P,        DOT,   KEY_A, KEY_E, KEY_I, KEY_H, ENT,
      CTRL_CMD, KEY_V, KEY_W, KEY_M, KEY_C, KEY_B,        SCLN,  COMMA, SLSH,  APOS,  KEY_X, RSFT,
                       L(2),  SPC,   L(1),  L(4),         RSFT,  L(2),  KEY_T, L(1)
  ),
};

const keycode_t layers[NUM_LAYERS][NUM_KEYS] = {
  MAKE_LAYER(
    ESC,      ___, ___, ___, ___, ___,        ___, ___, ___, ___, ___, BSPC,
    TAB,      ___, ___, ___, ___, ___,        ___, ___, ___, ___, ___, ENT,
    CTRL_CMD, ___, ___, ___, ___, ___,        ___, ___, ___, ___, ___, RSFT,
                   ___, ___, ___, ___,        ___, ___, ___, ___
),

  // Symbols (1__)
  MAKE_LAYER(
    TILD, LBRK,  LPAR,  RPAR,   RBRK, BSLSH,      AT,   GRV,  APOS, DQUOT, XXX,  ___,
    CLN,  DLR,   LBRC,  RBRC,   QUES, EXCL,       AMPR, SLSH, EQ,   DOT,   SCLN, ___,
    HASH, CARET, COMMA, UNDERS, PERC, ASTER,      PIPE, LT,   PLUS, DASH,  GT,   ___,
                 ___,   ___,    ___,  ___,        ___,  ___,  ___,  ___
),

  // Navigation (_2__)
  MAKE_LAYER(
    ESC,    CTRL_CMD, BSPC, UP,   DEL,   END,        PGUP, UNDO, REDO,    SELECT_ALL, XXX, XXX,
    S(TAB), LSFT,     LEFT, DOWN, RIGHT, HOME,       PGDN, RSFT, ALT_CMD, CTRL_ALT,   XXX, XXX,
    XXX,    XXX,      XXX,  XXX,  XXX,   XXX,        XXX,  CUT,  COPY,    PASTE,      XXX, XXX,
                      ___,  ___,  ___,   ___,        ___,  ___,  ___,     ___
),

  // Functions (12__)
  MAKE_LAYER(
    XXX, XXX, KEY_F7, KEY_F8, KEY_F9, KEY_F10,       KEY_F7, KEY_F8, KEY_F9, KEY_F10, XXX, XXX,
    XXX, XXX, KEY_F4, KEY_F5, KEY_F6, KEY_F11,       KEY_F4, KEY_F5, KEY_F6, KEY_F11, XXX, XXX,
    XXX, XXX, KEY_F1, KEY_F2, KEY_F3, KEY_F12,       KEY_F1, KEY_F2, KEY_F3, KEY_F12, XXX, XXX,
              ___,    ___,    ___,    ___,           ___,  ___,  ___, ___
),
  
  // Numbers (__4_)
  MAKE_LAYER(
    ___, KEY_X, KEY_7, KEY_8, KEY_9, DASH,       ___,  ___,  ___, ___, ___, ___,
    ___, HASH,  KEY_4, KEY_5, KEY_6, DOT,        ___,  ___,  ___, ___, ___, ___,
    ___, KEY_0, KEY_1, KEY_2, KEY_3, DLR,        ___,  ___,  ___, ___, ___, ___,
                ___,   ___,   ___,   ___,        L(8), ___,  ___, ___
),

  // ?? (1_4_)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // System/Media (_24_)
  MAKE_LAYER(
    XXX, XXX, OSX,       OS_TOG,    LINUX,     XXX,              XXX,  BRI_D, BRI_U, XXX,   XXX, XXX,
    XXX, XXX, GOTO_QWER, GOTO_QGML, GOTO_NORD, GOTO_NORDM,       XXX,  VOLD,  VOLU,  XXX,   XXX, XXX,
    XXX, XXX, XXX,       XXX,       XXX,       XXX,              XXX,  M_PRV, M_PP,  M_NXT, XXX, XXX,
              ___,       ___,       ___,       ___,              ___,  ___,   ___,   ___
),

  // ?? (124_)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // System/Media (___8)
  MAKE_LAYER(
    XXX, XXX, OSX,       OS_TOG,    LINUX,     XXX,              XXX,  BRI_D, BRI_U, XXX,   XXX, XXX,
    XXX, XXX, GOTO_QWER, GOTO_QGML, GOTO_NORD, GOTO_NORDM,       XXX,  VOLD,  VOLU,  XXX,   XXX, XXX,
    XXX, XXX, XXX,       XXX,       XXX,       XXX,              XXX,  M_PRV, M_PP,  M_NXT, XXX, XXX,
              ___,       ___,       ___,       ___,              ___,  ___,   ___,   ___
),

  // ?? (1__8)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // ?? (_2_8)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // ?? (12_8)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // ?? (__48)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // ?? (1_48)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // ?? (_248)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

  // ?? (1248)
  MAKE_LAYER(
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
    ___, ___, ___,  ___, ___,  ___,        ___,  ___,  ___, ___, ___, ___,
              ___,  ___, ___,  ___,        ___,  ___,  ___, ___
),

};