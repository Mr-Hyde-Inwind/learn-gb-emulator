#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define BIT(x, n) (((x) >> (n)) & 1 ? 1 : 0)
#define BIT_SET(x, n, on) { if (on) (x) |= (1 << (n)); else (x) &= ~(1 << (n)); }
#define NO_IMPL { fprintf(stderr, "NOT IMPLEMENTED.\n"); exit(-7); }
