#include "pico/stdlib.h"
#include "hardware/gpio.h"

/*
pin #   	*2C33 pin	*RAM pins	signal description
-----   	---------	---------	------------------
1		50		5 (green)	(O) -write
2		64		C (cyan)	(O) VCC (+5VDC)
3		49		6 (blue)	(O) -scan media
4		32		1 (brown)	(O) VEE (ground)
5		52		3 (orange)	(O) write data
6		37		B (pink)	(I) motor on/battery good
7		47		8 (grey)	(I) -writable media
8		-		-       	(I) motor power (note 1)
9		51		4 (yellow)	(I) read  data
A		45		A (black)	(I) -media set
B		46		9 (white)	(I) -ready
C		48		7 (violet)	(O) -stop motor
*/

const uint LED_PIN = 25;

// Debug
const uint DEBUG_01_PIN = 0;
const uint DEBUG_02_PIN = 1;

// Output
const uint READY_PIN = 5; // IN, inverted
const uint MEDIA_SET_PIN = 3; // IN, inverted
const uint MOTOR_ON_PIN = 4; // IN
const uint READ_DATA_PIN = 6; // IN
const uint WRITABLE_MEDIA_PIN = 7; // IN, inverted

// Input
const uint WRITE_PIN = 10; // OUT, inverted
const uint SCAN_MEDIA_PIN = 9; // OUT, inverted
const uint WRITE_DATA_PIN = 8; // OUT
const uint STOP_MOTOR = 2; // OUT, inverted

const uint FLIP_BUTTON_PIN = 11;

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

void init_pins()
{
    init_out_pin(LED_PIN, false, true);

    init_out_pin(READY_PIN, true, false);
    init_out_pin(MEDIA_SET_PIN, true, false);
    init_out_pin(MOTOR_ON_PIN, true, false);
    init_out_pin(READ_DATA_PIN, true, false);
    init_out_pin(WRITABLE_MEDIA_PIN, true, false);

    init_in_pin(WRITE_PIN, false, true);
    init_in_pin(SCAN_MEDIA_PIN, false, true);
    init_in_pin(WRITE_DATA_PIN, false, true);
    init_in_pin(STOP_MOTOR, false, true);

    init_in_pin(FLIP_BUTTON_PIN, false, true);
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