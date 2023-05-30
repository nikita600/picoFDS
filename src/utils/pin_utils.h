#include "pico/stdlib.h"

static inline void setup_pull_resistor(uint pin_idx, bool disable_pull, bool pull_up)
{
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

static inline void setup_in_pin(uint pin_idx, bool disable_pull, bool pull_up)
{
    gpio_init(pin_idx);
    gpio_set_dir(pin_idx, GPIO_IN);
    setup_pull_resistor(pin_idx, disable_pull, pull_up);
}

static inline void setup_out_pin(uint pin_idx, bool disable_pull, bool pull_up)
{
    gpio_init(pin_idx);
    gpio_set_dir(pin_idx, GPIO_OUT);
    setup_pull_resistor(pin_idx, disable_pull, pull_up);
}
