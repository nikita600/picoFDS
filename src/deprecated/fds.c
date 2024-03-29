#include <memory.h>

#include "fds.h"
#include "storage/sd_card.h"

/*

void wait_for_button_down()
{
    while (true)
    {
        bool button_pressed = fds_is_button_down();
        if (button_pressed)
        {
            break;
        }
    }
}

void read_disk_side(int side, void* trg_buffer)
{
    memset(trg_buffer, 0, DISK_SIDE_SIZE);

    sd_card_read_file("ROM.fds", trg_buffer, DISK_SIDE_SIZE);
}

uint64_t current_time = 0;
uint64_t led_delay = 0;
uint64_t read_data_delay = 0;

enum FDS_STATE fds_state = fds_disk_set;
bool led_data = false;
bool read_data_clk = false;
bool is_gap_sending = false;
bool is_data_sending = false;
int bit_sended = 0;

int current_data = 0;

uint buffer_idx = 0;
unsigned char buffer[DISK_SIDE_SIZE];

static inline void print_fds_state()
{
    bool is_scan_media = fds_is_scan_media();
    bool is_stop_motor = fds_is_stop_motor();
    bool is_write = fds_is_write();
    bool write_data = fds_get_write_data();
    
#ifdef DEBUG
    printf("scan: %x, stop: %x, write: %x, write_data: %x\n",
     is_scan_media, is_stop_motor, is_write, write_data);
#endif

}

static inline void fds_read_next_byte()
{
    uint8_t byte = buffer[buffer_idx];
    buffer_idx++;

    current_data = byte;
    
    if (buffer_idx > DISK_SIDE_SIZE)
    {
        buffer_idx = 0;
    }
}

static inline void update_current_time()
{
    current_time = to_us_since_boot(get_absolute_time());
}

static inline void fds_update_read_data_delay()
{
    read_data_delay = to_us_since_boot(make_timeout_time_us(READ_DATA_DELAY0)); // read_data_clk ? READ_DATA_DELAY0 : READ_DATA_DELAY1
}

static inline void fds_set_state(enum FDS_STATE state)
{
    fds_state = state;

#ifdef DEBUG
    switch (fds_state)
    {
        case fds_idle:
            printf("fds_idle\n");
            break;

        case fds_disk_set:
            printf("fds_disk_set\n");
            break;

        case fds_scan_wait:
            printf("fds_scan_wait\n");
            break;

        case fds_motor_on:
            printf("fds_motor_on\n");
            break;

        case fds_send_gap:
            printf("fds_send_gap\n");
            break;

        case fds_ready:
            printf("fds_ready\n");
            break;

        case fds_read_write:
            printf("fds_read_write\n");
            break;

        case fds_read_write_end:
            printf("fds_read_write_end\n");
            break;

        default:
            printf("unknown fds state: 0x%x\n", state);
            break;
    }
#endif

}

static inline void init_fds()
{
    fds_set_media_set(false);
    fds_set_motor_on(false);
    fds_set_read_data(false);
    fds_set_ready(false);
    fds_set_writable(false);
} 

static inline void fds_set_disk_handler()
{
    wait_for_button_down();
    fds_set_media_set(true);
    fds_set_writable(true);
    fds_set_state(fds_scan_wait);
}

static inline void fds_scan_wait_handler()
{
    if (fds_is_scan_media() && !fds_is_stop_motor())
    {
        fds_set_state(fds_motor_on);
    }
}

static inline void fds_motor_on_handler()
{
    fds_set_motor_on(true);
    fds_update_read_data_delay();

    fds_read_next_byte();
    fds_set_state(fds_send_gap);
}

static inline void fds_send_gap_handler()
{
    while (fds_is_scan_media() && !fds_is_stop_motor())
    {
        //fds_wait_for_sample();
        fds_set_read_data(current_data & 1);

        //fds_wait_for_reset();
        fds_set_read_data(false);

        current_data >>= 1;
        bit_sended++;

        if (bit_sended == 8)
        {
            bit_sended = 0;

            fds_read_next_byte();

            if (buffer_idx == 0x2100)
            {
                fds_set_state(fds_ready);
                return;
            }

    #if DEBUG
            if (buffer_idx % 1000 == 0)
            {
                printf("sent: %i\n", buffer_idx);
                //print_fds_state();
            }
    #endif
        }
    }
}

static inline void fds_ready_handler()
{
    fds_read_next_byte();
    fds_set_ready(true);
    fds_set_state(fds_read_write);

    //sleep_ms(150);
}

static inline void fds_read_write_handler()
{
    if (!fds_is_scan_media() || fds_is_stop_motor())
    {
        return;
    }

    while (fds_is_scan_media() && !fds_is_stop_motor())
    {
        //fds_wait_for_sample();
        fds_set_read_data(current_data & 1);

        //fds_wait_for_reset();
        fds_set_read_data(false);

        current_data >>= 1;
        bit_sended++;

        if (bit_sended == 8)
        {
            bit_sended = 0;

            fds_read_next_byte();

            if (buffer_idx == 0)
            {
                fds_set_state(fds_read_write_end);
                return;
            }

    #if DEBUG
            if (buffer_idx % 1000 == 0)
            {
                printf("sent: %i\n", buffer_idx);
                //print_fds_state();
            }
    #endif
        }
    }

    //fds_set_state(fds_read_write_end);
    //printf("fail to read\n");
}

static inline void fds_stop_handler()
{
    fds_set_ready(false);
    fds_set_state(fds_idle);
}

static inline void led_blink_handler()
{
    if (current_time >= led_delay)
    {
        led_delay = to_us_since_boot(make_timeout_time_ms(LED_DELAY));
        gpio_put(LED_PIN, led_data);
        led_data = !led_data;
    }
}

static inline void fds_update()
{
    switch (fds_state)
    {
        case fds_idle:
            // no op
            break;

        case fds_disk_set:
            fds_set_disk_handler();
            break;

        case fds_scan_wait:
            fds_scan_wait_handler();
            break;

        case fds_motor_on:
            fds_motor_on_handler();
            break;

        case fds_send_gap:
            fds_send_gap_handler();
            break;

        case fds_ready:
            fds_ready_handler();
            break;

        case fds_read_write:
            fds_read_write_handler();
            break;

        case fds_read_write_end:
            fds_stop_handler();
            break;
    }
}

*/