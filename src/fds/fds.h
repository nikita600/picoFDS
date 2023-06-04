#include "pico\stdlib.h"
#include "..\types.h"
#include "ram_adapter.h"

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

    ushort gap_size;
    
    byte read_data;
    byte bit_count;

} fds_state;

void fds_state_reset(fds_state *state);
bool fds_state_update(fds_state *state);
