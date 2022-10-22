#include <memory.h>

#include "fds.h"
#include "sd_card.h"

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

void read_disk_side(int side, const void* buffer)
{
    memset(buffer, 0, DISK_SIDE_SIZE);

    read_from_sd_card(buffer, DISK_SIDE_SIZE);
}
