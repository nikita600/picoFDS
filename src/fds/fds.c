#include "fds.h"
#include <stdio.h>

#include <time.h>
#include <pico/stdlib.h>


#if DEBUG
clock_t start_clock;
clock_t current_clock;

clock_t get_clock()
{
    return (clock_t) time_us_64() / 10000;
}
#endif

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

static inline void fds_disk_set_state(fds_state *state)
{
    fds_ram_adapter_set_media_set(&state->ram_adapter, true);
    fds_ram_adapter_set_writable(&state->ram_adapter, true);

    state->current = fds_scan_wait;
}

static inline void fds_scan_wait_state(fds_state *state)
{
    if (fds_ram_adapter_is_scan_media(&state->ram_adapter) 
        && fds_ram_adapter_is_stop_motor(&state->ram_adapter))
    {
        state->current = fds_motor_on;
    }
}

static inline void fds_motor_on_state(fds_state *state)
{
    fds_ram_adapter_set_motor_on(&state->ram_adapter, true);

    state->gap_size = 28300;
    state->read_data = 0xFF;

    set_active_pio_ctx(&state->ram_adapter.read_pio_ctx, true);
    state->next_gap_state = fds_ready;
    state->current = fds_send_gap;
}

static inline void fds_send_gap_state(fds_state *state)
{
    fds_ram_adapter_set_read_data(&state->ram_adapter, 1);

    state->bit_count++;

    if (state->bit_count >= state->gap_size)
    {
        state->read_data = 0x80;
        state->current = fds_send_gap_end;
    }
}

static inline void fds_send_gap_end_state(fds_state *state)
{
    fds_ram_adapter_set_read_data(&state->ram_adapter, state->read_data & 1);

    state->read_data = state->read_data >> 1;
    state->bit_count++;

    if (state->bit_count >= 7)
    {
        state->current = state->next_gap_state;
    }
}

static inline void fds_ready_state(fds_state *state)
{
    fds_ram_adapter_set_ready(&state->ram_adapter, true);
         
    state->bit_count = 8;
    state->read_data = 0;
    state->gap_size = 0;

    state->block_idx = 0;
    state->block_offset = 0;
    
    state->current = fds_read_write;
}

static inline void fds_read_state(fds_state *state)
{
    if (state->bit_count >= 7)
    {
        fds_block_raw current_block = state->disk.raw_blocks[state->block_idx];

        if (state->block_offset >= current_block.size)
        {
            state->block_idx++;
            if (state->block_idx >= state->disk.raw_blocks_count)
            {
                fds_state_reset(state);
            }
            else
            {
                state->gap_size = 976;
                state->read_data = 0xFF;
                state->next_gap_state = fds_read_write;
                state->current = fds_send_gap;
            }
            return;
        }

        byte* data = current_block.data;
        state->read_data = data[state->block_offset];
        state->block_offset++;
        state->bit_count = 0;
    }

    fds_ram_adapter_set_read_data(&state->ram_adapter, state->read_data & 1);
    state->read_data = state->read_data >> 1;
    state->bit_count++;
}

static inline void fds_read_write_state(fds_state *state)
{
    if (!fds_ram_adapter_is_write(&state->ram_adapter))
    {
        fds_read_state(state);
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

bool fds_state_update(fds_state *state)
{
    switch (state->current)
    {
        case fds_idle:
            break;

        case fds_disk_set:
            fds_disk_set_state(state);
            break;

        case fds_scan_wait:
            fds_scan_wait_state(state);
            break;

        case fds_motor_on:
            fds_motor_on_state(state);
            break;

        case fds_send_gap:
            fds_send_gap_state(state);
            break;

        case fds_send_gap_end:
            fds_send_gap_end_state(state);
            break;

        case fds_ready:
            fds_ready_state(state);
            break;

        case fds_read_write:
            fds_read_write_state(state);
            break;

        case fds_read_write_end:
            break;
    }

    return true;
}