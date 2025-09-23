#include <Adafruit_TinyUSB.h>

#define EMPTY_ROW 0xC0

typedef signed char int8_t;
typedef short int16_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef uint16_t keycode_t;

const uint8_t NUM_ROWS = 4;
const uint8_t NUM_COLS = 6;

const uint8_t NUM_KEYS = NUM_COLS * NUM_ROWS;

const uint8_t COL_PINS[] = {14, 15, 26, 27, 28, 29};
const uint8_t ROW_PINS[] = {13, 12, 11, 10};

typedef uint32_t matrix_state_t;

#define keystate(matrix, row, col) (matrix & (1 << (row * NUM_COLS + col)))

matrix_state_t set_key(matrix_state_t matrix, uint8_t row, uint8_t col, bool pressed);

void setup_matrix();
matrix_state_t scan();

void setup_matrix()
{
  // Right Hand
  for (int r = 0; r < NUM_ROWS; r++)
  {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }
  for (int c = 0; c < NUM_COLS; c++)
  {
    digitalWrite(COL_PINS[c], HIGH);
    pinMode(COL_PINS[c], OUTPUT);
  }
}

matrix_state_t scan()
{
  matrix_state_t state = 0xC0C0C0C0;
  // Left hand
  for (int c = 0; c < NUM_COLS; c++)
  {
    digitalWrite(COL_PINS[c], LOW);
    // Delay for the pin to settle
    delayMicroseconds(30);
    // Read rows
    for (int r = 0; r < NUM_ROWS; r++)
    {
      state = set_key(state, r, c, !digitalRead(ROW_PINS[r]));
      // Serial.printf("LH: Row %d Col %d state: %02x\n", r, c, !digitalRead(LH_COL_PINS[c]));
    }
    digitalWrite(COL_PINS[c], HIGH);
  }

  return state;
}

matrix_state_t set_key(matrix_state_t matrix, uint8_t row, uint8_t col, bool pressed)
{
  uint16_t index = ((row * 8) + col);
  if (pressed)
  {
    return (matrix | (1U << index));
  }
  else
  {
    return matrix & ~(1U << index);
  }
}

void setup()
{
  // Serial.begin(9600);
  // Serial.println("Follower Keyboard");
  Serial1.begin(115200);

  setup_matrix();
}

static matrix_state_t prev_state = 0;
void loop()
{
  if (!Serial1.available())
  {
    delayMicroseconds(500);
    return;
  }

  // don't care about incoming data, just clear it out
  // it's only a signal that we should scan the matrix
  while(Serial1.available()) {
    Serial1.read();
  }

  static uint8_t row_state[NUM_ROWS] = {EMPTY_ROW, EMPTY_ROW, EMPTY_ROW, EMPTY_ROW};

  matrix_state_t state = scan();

  for (int i = 0; i < NUM_ROWS; i++)
  {
    uint8_t new_row = (state >> (i * 8)) & 0xFF;
    if (new_row != row_state[i])
    {
      row_state[i] = new_row;
    }
  }
  // Serial.println("Starting send:");
  Serial1.write(0xff); // start byte
  // Serial.printf("%0x %0x %0x %0x", row_state[0], row_state[1], row_state[2], row_state[3]);
  Serial1.write(row_state, sizeof(row_state));

  // Serial.printf("Sending rows info %08x:\n", state);
  // for(int i = 0; i < sizeof(row_state); i++) {
  //   Serial.printf("%02x\n", row_state[i]);
  // }
  Serial1.write(0xee); // end byte
  // clean out the write buffer so we don't loop until everything is sent
  Serial1.flush();
  // Serial.println("End send");

  // if (prev_state != state)
  // {
  //   Serial.printf("%08x\n", state);
  //   for (int r = 0; r < NUM_ROWS; r++)
  //   {
  //     for (int c = 0; c < NUM_COLS; c++)
  //     {
  //       Serial.print(((state >> (r * 8 + c)) & 0x1) ? 'X' : '.');
  //     }
  //     Serial.println();
  //   }
  //   Serial.println();
  // }

  prev_state = state;
}
