#ifndef RAM_ADAPTER_H_
#define RAM_ADAPTER_H_

#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "../utils/pin_utils.h"
#include "../utils/pio_utils.h"

typedef struct
{
    uint motor_on_pin;
    uint stop_motor_pin;

    uint media_set_pin;
    uint writable_data_pin;

    uint ready_pin;
    uint scan_media_pin;
    uint write_pin;

    uint read_data_pin;
    uint write_data_pin;

} fds_ram_adapter;

fds_ram_adapter create_fds_ram_adapter();

void setup_fds_ram_adapter(fds_ram_adapter *ram_adapter, pio_ctx *fds_read_ctx);

// Inputs
void fds_ram_adapter_set_ready(fds_ram_adapter *ram_adapter, bool state);
void fds_ram_adapter_set_media_set(fds_ram_adapter *ram_adapter, bool state);
void fds_ram_adapter_set_motor_on(fds_ram_adapter *ram_adapter, bool state);
void fds_ram_adapter_set_read_data(fds_ram_adapter *ram_adapter, bool state);
void fds_ram_adapter_set_writable(fds_ram_adapter *ram_adapter, bool state);

// Outputs
bool fds_ram_adapter_is_scan_media(fds_ram_adapter *ram_adapter);
bool fds_ram_adapter_is_stop_motor(fds_ram_adapter *ram_adapter);
bool fds_ram_adapter_is_write(fds_ram_adapter *ram_adapter);
bool fds_ram_adapter_get_write_data(fds_ram_adapter *ram_adapter);

#endif