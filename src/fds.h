#ifndef FDS_H_
#define FDS_H_

#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Constants
#define DISK_SIDE_SIZE 0x23000 //0x10000 //65500
#define TRANSFER_FREQ_HZ 96400 / 2

enum FDS_STATE
{
    fds_idle,
    fds_disk_set,
    fds_scan_wait,
    fds_motor_on,
    fds_send_gap,
    fds_ready,
    fds_read_write,
    fds_read_write_end
};

#define LED_PIN 25
#define FLIP_BUTTON_PIN 11

// Debug
#define DEBUG_01_PIN 0
#define DEBUG_02_PIN 1

#define READY_PIN 7             // IN, inverted
#define MEDIA_SET_PIN 9         // IN, inverted
#define MOTOR_ON_PIN 8          // IN
#define READ_DATA_PIN 6         // IN
#define WRITABLE_MEDIA_PIN 5    // IN, inverted

#define SCAN_MEDIA_PIN 3    // OUT, inverted
#define STOP_MOTOR 10       // OUT, inverted
#define WRITE_PIN 2         // OUT, inverted
#define WRITE_DATA_PIN 4    // OUT

static inline void init_in_pin(uint pin_idx, bool disable_pull, bool pull_up)
{
    gpio_init(pin_idx);
    gpio_set_dir(pin_idx, GPIO_IN);

    if (disable_pull)
    {
        gpio_disable_pulls(pin_idx);
    }
    else
    {
        if (pull_up)
        {
            gpio_pull_up(pin_idx);
        }
        else
        {
            gpio_pull_down(pin_idx);
        }
    }
}

static inline void init_out_pin(uint pin_idx, bool disable_pull, bool pull_up)
{
    gpio_init(pin_idx);
    gpio_set_dir(pin_idx, GPIO_OUT);

    if (disable_pull)
    {
        gpio_disable_pulls(pin_idx);
    }
    else
    {
        if (pull_up)
        {
            gpio_pull_up(pin_idx);
        }
        else
        {
            gpio_pull_down(pin_idx);
        }
    }
}

static inline void init_pins()
{
    init_out_pin(LED_PIN, false, true);
    init_in_pin(FLIP_BUTTON_PIN, false, true);

    init_out_pin(READY_PIN, false, true);
    init_out_pin(MEDIA_SET_PIN, false, true);
    init_out_pin(MOTOR_ON_PIN, false, false);
    init_out_pin(READ_DATA_PIN, false, false);
    init_out_pin(WRITABLE_MEDIA_PIN, false, true);

    init_in_pin(WRITE_PIN, false, false);
    init_in_pin(SCAN_MEDIA_PIN, false, false);
    init_in_pin(WRITE_DATA_PIN, false, false);
    init_in_pin(STOP_MOTOR, false, false);
}

// FLIP DISK BUTTON
static inline bool fds_is_button_down()
{
    return !gpio_get(FLIP_BUTTON_PIN);
}

// Inputs
static inline void fds_set_ready(bool state)
{
    gpio_put(READY_PIN, !state);
}
static inline void fds_set_media_set(bool state)
{
    gpio_put(MEDIA_SET_PIN, !state);
}
static inline bool fds_set_motor_on(bool state)
{
    gpio_put(MOTOR_ON_PIN, state);
}
static inline bool fds_set_read_data(bool state)
{
    gpio_put(READ_DATA_PIN, state);
}
static inline bool fds_set_writable(bool state)
{
    gpio_put(WRITABLE_MEDIA_PIN, !state);
}

// Outputs
static inline bool fds_is_scan_media()
{
    return !gpio_get(SCAN_MEDIA_PIN);     
}
static inline bool fds_is_stop_motor()
{
    return !gpio_get(STOP_MOTOR);     
}
static inline bool fds_is_write()
{
    return !gpio_get(WRITE_PIN);     
}
static inline bool fds_get_write_data()
{
    return gpio_get(WRITE_DATA_PIN);     
}

void read_disk_side(int side, void* buffer);

void wait_for_button_down();

#endif