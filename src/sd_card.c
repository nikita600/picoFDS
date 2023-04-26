#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "hw_config.h"

#include "sd_card.h"

void read_from_sd_card(const void* buffer, uint size)
{
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr)
    {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    FIL fil;
    const char* const filename = "ROM.DKA";
    fr = f_open(&fil, filename, FA_READ);
    if (FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);

    UINT bw = 0;
    if (f_read(&fil, buffer, size, &bw) != FR_OK)
    {
        printf("f_printf failed\n");
    }

    fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    f_unmount(pSD->pcName);
}

void write_to_sd_card(const void* buffer, uint size)
{
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr)
    {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    FIL fil;
    const char* const filename = "fds_dump.bin";
    fr = f_open(&fil, filename, FA_CREATE_NEW | FA_WRITE);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    }

    UINT bw = 0;

    f_write(&fil, buffer, size, &bw);

    fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    f_unmount(pSD->pcName);
}