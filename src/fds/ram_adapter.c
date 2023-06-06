#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "read_data.pio.h"
#include "ram_adapter.h"

#define PIO_FDS_CLOCK 800000

fds_ram_adapter create_fds_ram_adapter()
{
    fds_ram_adapter ram_adapter;
    
    ram_adapter.motor_on_pin = 8;
    ram_adapter.stop_motor_pin = 10;
    
    ram_adapter.media_set_pin = 9;
    ram_adapter.writable_data_pin = 5;

    ram_adapter.ready_pin = 7;
    ram_adapter.scan_media_pin = 3;
    ram_adapter.write_pin = 2;

    ram_adapter.read_data_pin = 6;
    ram_adapter.write_data_pin = 4;

    setup_fds_ram_adapter(&ram_adapter);

    return ram_adapter;
}

uint setup_fds_read_pio(PIO pio, fds_ram_adapter *fds_ram_adapter)
{
    uint offset = pio_add_program(pio, &fds_read_data_program);
    uint sm = pio_claim_unused_sm(pio, true);
    fds_read_data_program_init(pio, sm, offset, fds_ram_adapter->read_data_pin);

    uint len = fds_read_data_program.length;

    uint clock_hz = clock_get_hz(clk_sys);
    float pio_clk_div = (float)(clock_hz / (float)PIO_FDS_CLOCK);
    pio_sm_set_clkdiv(pio, sm, pio_clk_div);

    return sm;
}

void setup_fds_ram_adapter(fds_ram_adapter *ram_adapter)
{
    setup_out_pin(ram_adapter->ready_pin, false, true);
    setup_out_pin(ram_adapter->media_set_pin, false, true);
    setup_out_pin(ram_adapter->motor_on_pin, false, false);
    setup_out_pin(ram_adapter->read_data_pin, false, false);
    setup_out_pin(ram_adapter->writable_data_pin, false, true);

    setup_in_pin(ram_adapter->write_pin, false, false);
    setup_in_pin(ram_adapter->scan_media_pin, false, false);
    setup_in_pin(ram_adapter->write_data_pin, false, false);
    setup_in_pin(ram_adapter->stop_motor_pin, false, false);
    
    ram_adapter->read_pio_ctx.pio = pio0;
    ram_adapter->read_pio_ctx.sm = setup_fds_read_pio(ram_adapter->read_pio_ctx.pio, ram_adapter);
}

// Inputs
void fds_ram_adapter_set_ready(fds_ram_adapter *ram_adapter, bool state)
{
    gpio_put(ram_adapter->ready_pin, !state);
}

void fds_ram_adapter_set_media_set(fds_ram_adapter *ram_adapter, bool state)
{
    gpio_put(ram_adapter->media_set_pin, !state);
}

void fds_ram_adapter_set_motor_on(fds_ram_adapter *ram_adapter, bool state)
{
    gpio_put(ram_adapter->motor_on_pin, state);
}

void fds_ram_adapter_set_read_data(fds_ram_adapter *ram_adapter, bool state)
{
    //gpio_put(ram_adapter->read_data_pin, state);
    pio_sm_put_blocking(ram_adapter->read_pio_ctx.pio, ram_adapter->read_pio_ctx.sm, state);

    //while (pio_sm_is_tx_fifo_full(ram_adapter->read_pio_ctx.pio, ram_adapter->read_pio_ctx.sm))
    //{
    //    tight_loop_contents();
    //}
    //pio_sm_put(ram_adapter->read_pio_ctx.pio, ram_adapter->read_pio_ctx.sm, state);
}

void fds_ram_adapter_set_writable(fds_ram_adapter *ram_adapter, bool state)
{
    gpio_put(ram_adapter->writable_data_pin, !state);
}

// Outputs
bool fds_ram_adapter_is_scan_media(fds_ram_adapter *ram_adapter)
{
    return !gpio_get(ram_adapter->scan_media_pin);     
}

bool fds_ram_adapter_is_stop_motor(fds_ram_adapter *ram_adapter)
{
    return !gpio_get(ram_adapter->stop_motor_pin);     
}

bool fds_ram_adapter_is_write(fds_ram_adapter *ram_adapter)
{
    return !gpio_get(ram_adapter->write_pin);     
}

bool fds_ram_adapter_get_write_data(fds_ram_adapter *ram_adapter)
{
    return gpio_get(ram_adapter->write_data_pin);     
}
