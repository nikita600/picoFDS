#include <stdio.h>
#include <memory.h>
//
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "hw_config.h"

#include "pins.h"
#include "fds_rom_a.h"
#include "fds_rom_b.h"

const uint32_t cycle_time_sleep_us = 5;

void rom_test()
{
    unsigned char buffer[65500UL + 1];

    for (int i = 0; i < fds_rom_a_size; ++i)
    {
        unsigned char rom_byte = fds_rom_a[i];
        for (int b = 0; b < 8; ++b)
        {
            buffer[i] = (unsigned char)(fds_rom_a[i] << 1);
        }
    }

    for (int i = 0; i < fds_rom_a_size; ++i)
    {
        unsigned char rom_byte = fds_rom_b[i];
        for (int b = 0; b < 8; ++b)
        {
            buffer[i] = (unsigned char)(fds_rom_b[i] << 1);
        }
    }
}

void init_in_pin(uint pin_idx)
{
    gpio_init(pin_idx);
    gpio_set_dir(pin_idx, GPIO_IN);
    gpio_pull_down(pin_idx);
}

void init_out_pin(uint pin_idx)
{
    gpio_init(pin_idx);
    gpio_set_dir(pin_idx, GPIO_OUT);
}

void init_pins()
{
    init_out_pin(LED_PIN);

    init_in_pin(READY_PIN);
    init_in_pin(MEDIA_SET_PIN);
    init_in_pin(MOTOR_ON_PIN);
    init_in_pin(READ_DATA_PIN);
    init_in_pin(WRITABLE_MEDIA_PIN);

    init_in_pin(WRITE_PIN);
    init_in_pin(SCAN_MEDIA_PIN);
    init_in_pin(WRITE_DATA_PIN);
    init_in_pin(STOP_MOTOR);
}

bool is_ready()
{
    return !gpio_get(READY_PIN);
}

bool is_media_set()
{
    return !gpio_get(MEDIA_SET_PIN);
}

bool is_motor_on()
{
    return gpio_get(MOTOR_ON_PIN);
}

bool is_scan_media()
{
    return !gpio_get(SCAN_MEDIA_PIN);     
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
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);

    UINT bw = 0;

    f_write(&fil, buffer, size, &bw);

    fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    f_unmount(pSD->pcName);
}

int main() 
{
    stdio_init_all();
    time_init();

    init_pins();

    uint buffer_idx = 0;
    unsigned char buffer[131000];
    memset(buffer, 0, 131000);

    bool write_pending = false;

    uint byte_idx = 0;
    unsigned char byte_data = 0;

    gpio_put(LED_PIN, 1);

    while (true) 
    {
        if (is_scan_media())
        {
            gpio_put(LED_PIN, 0);
            write_pending = true;

            bool data = gpio_get(READ_DATA_PIN);

            byte_idx++;
            byte_data = (unsigned char)(byte_data | (data << byte_idx));

            if (byte_idx == 8)
            {
                buffer[buffer_idx] = byte_data;
                
                byte_idx = 0;
                byte_data = 0;
                
                buffer_idx++;
                if (buffer_idx == 131000)
                {
                    buffer_idx = 0;
                }
            }

            sleep_us(10);
        }
        else
        {
            if (write_pending)
            {
                // Write File

                write_to_sd_card(buffer, 131000);

                memset(buffer, 0, 131000);
                write_pending = false;
                gpio_put(LED_PIN, 1);
            }
        }

        //gpio_put(LED_PIN, 0);
        //sleep_us(cycle_time_sleep_us);
        //gpio_put(LED_PIN, 1);
        //sleep_us(cycle_time_sleep_us);
    }
}