#include <stdio.h>
#include <memory.h>
//
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "pins.h"
#include "fds.h"
#include "sd_card.h"

#define ONE_SECOND 1000000
#define READ_DATA_DELAY0 5
#define READ_DATA_DELAY1 6
#define LED_DELAY 250
#define GAP_SIZE 28300

static inline init_fds()
{
    fds_set_media_set(false);
    fds_set_motor_on(true);
    fds_set_read_data(false);
    fds_set_ready(false);
    fds_set_writable(false);
} 

int main() 
{
    stdio_init_all();
    time_init();

    init_pins();
    init_fds();
    
    gpio_put(LED_PIN, 0);

    bool led_data = false;
    bool read_data_clk = false;
    
    bool is_gap_sending = false;
    bool is_data_sending = false;

    int bit_sended = 0;
    int current_byte = 0;

    uint64_t led_delay = to_us_since_boot(make_timeout_time_ms(LED_DELAY));
    uint64_t read_data_delay = to_us_since_boot(make_timeout_time_us(READ_DATA_DELAY0));

    //wait_for_button_down();

    uint buffer_idx = 0;
    unsigned char buffer[DISK_SIDE_SIZE];
    read_disk_side(0, buffer);
    gpio_put(LED_PIN, 1);

    fds_set_media_set(true);
    fds_set_writable(true);

    while (true) 
    {
        uint64_t current_time = to_us_since_boot(get_absolute_time());

        if (!is_data_sending)
        {
            if (fds_is_scan_media() && !fds_is_stop_motor())
            {
                fds_set_motor_on(true);

                is_data_sending = true;
                is_gap_sending = true;

                current_byte = buffer[buffer_idx];
                buffer_idx++;
                printf("start sent: %i\n", buffer_idx);
                if (buffer_idx > DISK_SIDE_SIZE)
                {
                    buffer_idx = 0;
                }
            }
        }
        else
        {
            // DATA
            if (current_time >= read_data_delay)
            {
                read_data_delay = to_us_since_boot(make_timeout_time_us(read_data_clk ? READ_DATA_DELAY1 : READ_DATA_DELAY1));

                read_data_clk = !read_data_clk;
                    gpio_put(LED_PIN, read_data_clk);

                    if (is_gap_sending)
                    {
                        fds_set_read_data(read_data_clk);

                        bit_sended++;
                        if (bit_sended >= GAP_SIZE)
                        {
                            bit_sended = 0;
                            is_gap_sending = false;
                        }
                    }
                    else
                    {
                        if (read_data_clk)
                        {
                            fds_set_read_data(read_data_clk);
                        }
                        else
                        {
                            if (!fds_is_write())
                            {
                                fds_set_read_data((current_byte >> bit_sended) & 1);
                                bit_sended++;

                            if (bit_sended == 8)
                            {
                                bit_sended = 0;
                                current_byte = buffer[buffer_idx];
                                buffer_idx++;

                                if (buffer_idx % 1000 == 0)
                                    printf("sent: %i\n", buffer_idx);
                                
                                if (buffer_idx > DISK_SIDE_SIZE)
                                {
                                    buffer_idx = 0;
                                    fds_set_motor_on(false);
                                    is_data_sending = false;
                                    is_gap_sending = false;

                                }
                            }
                            }
                            else
                            {
                                fds_set_read_data(read_data_clk);
                            }
                            
                        }
                    }
            }

            if (!fds_is_scan_media() || fds_is_stop_motor())
            {
                fds_set_motor_on(false);

                buffer_idx = 0;
                is_data_sending = false;
                is_gap_sending = false;

                gpio_put(LED_PIN, false);
            }
        }

        //if (current_time >= led_delay)
        //{
        //    led_delay = to_us_since_boot(make_timeout_time_ms(LED_DELAY));
//
        //    gpio_put(LED_PIN, led_data);
        //    led_data = !led_data;
        //}
    }
}