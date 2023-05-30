#include <stdio.h>
#include <memory.h>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "fds_read.pio.h"

#include "fds.h"
#include "sd_card.h"

#include "fds/ram_adapter.h"


uint buffer_idx = 0;
uint buffer_byte = 0;
unsigned char buffer[DISK_SIDE_SIZE];

//#define DEBUG 1

static inline void led_set_active(bool active)
{
    gpio_put(LED_PIN, active);
}

#define TEST_WORD 0xAAAA

int main() 
{
    stdio_init_all();

    pio_ctx fds_read_ctx;
    fds_ram_adapter ram_adapter = create_fds_ram_adapter();
    setup_fds_ram_adapter(&ram_adapter, &fds_read_ctx);

    //read_disk_side(0, buffer);
    
    led_set_active(true);
    
    uint data = TEST_WORD;
    uint bit_count = 0;

    while (true)
    {
        pio_sm_put_blocking(fds_read_ctx.pio, fds_read_ctx.sm, (data & 1));

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