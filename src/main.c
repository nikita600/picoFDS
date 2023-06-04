#include <stdio.h>
#include <memory.h>

#include "stdlib.h"

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "read_data.pio.h"
#include "fds/ram_adapter.h"
#include "fds/disk.h"

//#define DEBUG 1

static inline void led_set_active(bool active)
{
    gpio_put(LED_PIN, active);
}

#define TEST_WORD 0xAAAA

void read_rom_from_sd()
{
    sd_card_file_ctx file_ctx = sd_card_open_file("ROM.fds", FA_READ);

    fds_disk_info_block disk_info_block = read_info_block(&file_ctx);
    fds_disk_file_amount_block file_amount_block = read_file_amount_block(&file_ctx);
    for (int i = 0; i < file_amount_block.file_amount; ++i)
    {
        fds_disk_file_header_block file_header_block = read_file_header_block(&file_ctx);
        fds_disk_file_data_block file_data_block = read_file_data_block(&file_ctx, file_header_block.file_size);
        free(file_data_block.disk_data);
    }
    
    sd_card_close_file(file_ctx);
}

int main() 
{
    stdio_init_all();

    fds_ram_adapter ram_adapter = create_fds_ram_adapter();

    read_rom_from_sd();

    led_set_active(true);
    
    uint data = TEST_WORD;
    uint bit_count = 0;

    while (true)
    {
        fds_ram_adapter_set_read_data(&ram_adapter, (data & 1));

        data = data >> 1;
        bit_count++;
        if (bit_count >= 16)
        {
            data = TEST_WORD;
            bit_count = 0;
        }
    }
}

/*
int main() 
{
    stdio_init_all();
    time_init();

    init_pins();
    init_fds();
    
    gpio_put(LED_PIN, 0);

    led_delay = to_us_since_boot(make_timeout_time_ms(LED_DELAY));

    //wait_for_button_down();
    
    read_disk_side(0, buffer);

    gpio_put(LED_PIN, 1);

    while (true) 
    {
        update_current_time();

        fds_update();

        led_blink_handler();
    }
}
*/