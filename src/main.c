#include <stdio.h>
#include <memory.h>

#include "stdlib.h"

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "read_data.pio.h"
#include "fds/disk.h"
#include "fds/fds.h"

//#define DEBUG 1

static inline void led_set_active(bool active)
{
    gpio_put(LED_PIN, active);
}

static inline void led_show_error()
{
    bool active = false;
    while (true)
    {
        led_set_active(active);
        sleep_ms(500);

        active = !active;
    }
    
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
    
    fds_state fds_state;
    fds_state.ram_adapter = create_fds_ram_adapter();
    
    fds_state_reset(&fds_state);

    read_rom_from_sd();
    fds_state.current = fds_disk_set;

    led_set_active(true);

    while (true)
    {
        if (!fds_state_update(&fds_state))
        {
            led_show_error();
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