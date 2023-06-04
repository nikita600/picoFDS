#include "fds.h"
#include <stdio.h>

void fds_state_reset(fds_state *state)
{
    fds_ram_adapter_set_media_set(&state->ram_adapter, false);
    fds_ram_adapter_set_motor_on(&state->ram_adapter, false);
    fds_ram_adapter_set_ready(&state->ram_adapter, false);
    fds_ram_adapter_set_writable(&state->ram_adapter, false);

    set_active_pio_ctx(&state->ram_adapter.read_pio_ctx, false);

    state->current = fds_idle;
}

/*

uint data = TEST_WORD;
uint bit_count = 0;

void fds_send_read_data(fds_ram_adapter *ram_adapter)
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
*/

bool fds_state_update(fds_state *state)
{
    switch (state->current)
    {
        case fds_idle:
        {

        }
        break;

        case fds_disk_set:
        {
            fds_ram_adapter_set_media_set(&state->ram_adapter, true);
            fds_ram_adapter_set_writable(&state->ram_adapter, true);

            state->current = fds_scan_wait;
        }
        break;

        case fds_scan_wait:
        {
            if (fds_ram_adapter_is_scan_media(&state->ram_adapter) 
                && fds_ram_adapter_is_stop_motor(&state->ram_adapter))
            {
                state->current = fds_motor_on;
            }
        }
        break;

        case fds_motor_on:
        {
            fds_ram_adapter_set_motor_on(&state->ram_adapter, true);

            state->gap_size = 28300;
            state->read_data = 0xFF;

            set_active_pio_ctx(&state->ram_adapter.read_pio_ctx, true);
            state->current = fds_send_gap;
        }
        break;

        case fds_send_gap:
        {
            fds_ram_adapter_set_read_data(&state->ram_adapter, state->read_data & 1);

            state->read_data = state->read_data >> 1;
            state->bit_count++;

            if (state->bit_count >= state->gap_size)
            {
                state->read_data = 0x80;
                state->current = fds_send_gap_end;
            }
            else if (state->bit_count % 8 == 0)
            {
                state->read_data = 0xFF;
            }
        }
        break;

        case fds_send_gap_end:
        {
            fds_ram_adapter_set_read_data(&state->ram_adapter, state->read_data & 1);

            state->read_data = state->read_data >> 1;
            state->bit_count++;

            if (state->bit_count >= 8)
            {
                state->current = fds_ready;
            }
        }
        break;

        case fds_ready:
        {
            fds_ram_adapter_set_ready(&state->ram_adapter, true);
            state->current = fds_read_write;
        }
        break;

        case fds_read_write:
        {
            if (!fds_ram_adapter_is_write(&state->ram_adapter))
            {
                // READ HANDLER
            }
            else
            {
                // WRITE HANDLER
            }

            if (!fds_ram_adapter_is_scan_media(&state->ram_adapter) 
                || fds_ram_adapter_is_stop_motor(&state->ram_adapter))
            {
                fds_state_reset(state);
            }
        }
        break;
    }

    return true;
}