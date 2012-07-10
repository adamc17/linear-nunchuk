#include <stdint.h>

//Declares Output Pins
enum {
  pinA = 2,
  pinB = 3
};

typedef int16_t EncoderType;

void setupEncoder();
EncoderType getEncoderValue();
