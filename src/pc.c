
#include "pc.h"

bool
  enable_postfix = 0, use_separator1 = 0, use_separator2 = 0;

unsigned int
  angle_mode = ANGLE_RAD, number_format = MODE_NORM,
  fix_digits = 4, base = 10,
  separator_freq1 = 3, separator_freq2 = 10;

char separator1 = '\'', separator2 = '.';

atomic_uint cerrno = 0;