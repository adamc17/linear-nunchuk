#include <stdint.h>

//Declares Output Pins
enum {
  pinA = 4,
  pinB = 5
};

typedef int16_t EncoderType;

void setupEncoder();
EncoderType getEncoderValue();
