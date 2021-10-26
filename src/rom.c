#include <stdio.h>

#include "rom.h"

size_t rom_load(const char *filename, uint8_t **dst) {
  *dst = (uint8_t*)malloc(BUFSIZ);
  size_t ret = 0;
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error opening rom file: %s\n", filename);
    return ret;
  }
  ret = fread(*dst, sizeof(uint8_t), BUFSIZ, fp);
  if (ferror(fp)) {
    fprintf(stderr, "Error after reading rom file: %s\n", filename);
  }
  fclose(fp);

  return ret;
}

