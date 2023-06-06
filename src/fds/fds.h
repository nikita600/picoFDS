#include "pico\stdlib.h"
#include "..\types.h"
#include "ram_adapter.h"
#include "disk.h"

enum FDS_STATE_ID
{
    fds_idle,
    fds_disk_set,
    fds_scan_wait,
    fds_motor_on,
    fds_send_gap,
    fds_send_gap_end,
    fds_ready,
    fds_read_write,
    fds_read_write_end
};

typedef struct
{
    enum FDS_STATE_ID current;
    fds_ram_adapter ram_adapter;

    fds_disk disk;

    uint block_idx;
    uint block_offset;

    uint gap_size;
    enum FDS_STATE_ID next_gap_state;

    byte read_data;
    uint bit_count;

} fds_state;

void fds_state_reset(fds_state *state);
bool fds_state_update(fds_state *state);
