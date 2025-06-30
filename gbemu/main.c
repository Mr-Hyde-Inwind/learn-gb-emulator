#include <common.h>
#include <cart.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("No ROM file supplied.");
    return -1;
  }

  cart_load(argv[1]);

  return 0;
}