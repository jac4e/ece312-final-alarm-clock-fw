#include "common.h"
#include <avr/cpufunc.h>

void common_nop() {
    _NOP();
}