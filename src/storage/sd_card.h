#include "pico/stdlib.h"

#include "ff.h"
#include "hw_config.h"

typedef struct 
{
    sd_card_t* sd_card;
    FIL file;

} sd_card_file_ctx;

sd_card_t* sd_card_mount();
void sd_card_unmount(sd_card_t* sd_card);

sd_card_file_ctx sd_card_open_file(const char* filename, BYTE mode);
void sd_card_close_file(sd_card_file_ctx file);

void sd_card_read_file(const char* filename, void* buffer, uint size);
void sd_card_write_file(const char* filename, void* buffer, uint size);
