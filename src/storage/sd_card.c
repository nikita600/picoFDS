#include <stdio.h>
#include "f_util.h"

#include "sd_card.h"

sd_card_t* sd_card_mount()
{
    sd_card_t *sd_card = sd_get_by_num(0);
    FRESULT fr = f_mount(&sd_card->fatfs, sd_card->pcName, 1);
    if (FR_OK != fr)
    {
        panic("sd_card_mount::f_mount error: %s (%d)", FRESULT_str(fr), fr);
    }
    return sd_card;
}

void sd_card_unmount(sd_card_t* sd_card)
{
    f_unmount(sd_card->pcName);
}

sd_card_file_ctx sd_card_open_file(const char* filename, BYTE mode)
{
    sd_card_file_ctx ctx;
    
    ctx.sd_card = sd_card_mount();

    FRESULT fr = f_open(&ctx.file, filename, mode);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        panic("sd_card_open_file::f_open(%s) error: %s (%d)", filename, FRESULT_str(fr), fr);
    }

    return ctx;
}

void sd_card_close_file(sd_card_file_ctx ctx)
{
    FRESULT fr = f_close(&ctx.file);
    if (FR_OK != fr)
    {
        panic("sd_card_open_file::f_close error: %s (%d)", FRESULT_str(fr), fr);
    }

    sd_card_unmount(ctx.sd_card);
}

void sd_card_read_file(const char* filename, void* buffer, uint size)
{
    sd_card_file_ctx ctx = sd_card_open_file(filename, FA_READ);

    UINT bw = 0;
    if (f_read(&ctx.file, buffer, size, &bw) != FR_OK)
    {
        panic("f_read failed");
    }

    sd_card_close_file(ctx);
}

void sd_card_write_file(const char* filename, void* buffer, uint size) 
{
    sd_card_file_ctx ctx = sd_card_open_file(filename, FA_CREATE_NEW | FA_WRITE);

    UINT bw = 0;
    f_write(&ctx.file, buffer, size, &bw);

    sd_card_close_file(ctx);
}

void sd_card_read_bytes(sd_card_file_ctx* ctx, void* buffer, uint size)
{
    UINT bw = 0;
    FRESULT result = f_read(&ctx->file, buffer, size, &bw);
    if (result != FR_OK)
    {
        panic("sd_card_read_bytes:: fail to read, reason: %s", FRESULT_str(result));
    }
    
    /*
    uint pos = f_tell(&ctx->file);
    result = f_lseek(&ctx->file, pos + size);
    if (result != FR_OK)
    {
        panic("sd_card_read_bytes:: fail to seek, reason: %s", FRESULT_str(result));
    }
    */
}