#include "pico/stdlib.h"

void read_from_sd_card(const char* filename, const void* buffer, uint size);
void write_to_sd_card(const char* filename, const void* buffer, uint size);
