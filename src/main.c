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

#define FDS_CLOCK_HZ 96400
#define FDS_CLOCK_US 10.3734
#define PIO_FDS_CLOCK 800000

uint buffer_idx = 0;
uint buffer_byte = 0;
unsigned char buffer[DISK_SIDE_SIZE];

//#define DEBUG 1

static inline void led_set_active(bool active)
{
    gpio_put(LED_PIN, active);
}

static inline uint setup_fds_read_pio(PIO pio, uint read_data_pin)
{
    uint offset = pio_add_program(pio, &fds_read_program);
    uint sm = pio_claim_unused_sm(pio, true);
    fds_read_program_init(pio, sm, offset, read_data_pin);

    uint len = fds_read_program.length;

    uint clock_hz = clock_get_hz(clk_sys);
    float pio_clk_div = (float)(clock_hz / (float)PIO_FDS_CLOCK);//FDS_CLOCK_HZ);
    pio_sm_set_clkdiv(pio, sm, pio_clk_div);

    return sm;
}

#define TEST_WORD 0xAAAA

int main() 
{
    stdio_init_all();

    read_disk_side(0, buffer);

    PIO fds_read_pio = pio0;
    uint fds_read_sm = setup_fds_read_pio(fds_read_pio, READ_DATA_PIN);
    
    led_set_active(true);
    
    uint data = TEST_WORD;
    uint bit_count = 0;

    while (true)
    {
        pio_sm_put_blocking(fds_read_pio, fds_read_sm, (data & 1));

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