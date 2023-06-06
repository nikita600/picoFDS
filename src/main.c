#include <stdio.h>
#include <memory.h>

#include "stdlib.h"

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "read_data.pio.h"
#include "fds/fds.h"

//#define DEBUG 1

static inline void led_set_active(bool active)
{
    gpio_put(LED_PIN, active);
}

static inline void led_show_error()
{
    bool active = false;
    while (true)
    {
        led_set_active(active);
        sleep_ms(500);

        active = !active;
    }
    
}

#define TEST_WORD 0xAAAA

static inline fds_block_raw create_fds_block_raw(void *data, uint size)
{
    fds_block_raw block_raw;
    block_raw.size = size;
    block_raw.data = (byte*)data;
    return block_raw;
}

fds_disk read_rom_from_sd()
{
    sd_card_file_ctx file_ctx = sd_card_open_file("ROM.fds", FA_READ);

    fds_disk disk;
    
    disk.info_block = read_info_block(&file_ctx);
    disk.file_amount_block = read_file_amount_block(&file_ctx);

    disk.files_count = disk.file_amount_block.file_amount;

    fds_disk_file_header_block file_headers[disk.files_count];
    disk.file_header_blocks = file_headers;

    fds_disk_file_data_block data_blocks[disk.files_count];
    disk.file_data_blocks = data_blocks;

    disk.raw_blocks_count = disk.files_count + 2;
    fds_block_raw raw_data_blocks[disk.raw_blocks_count];

    uint block_idx = 0;
    raw_data_blocks[block_idx] = create_fds_block_raw(&disk.info_block, sizeof(fds_disk_info_block));
    block_idx++;

    raw_data_blocks[block_idx] = create_fds_block_raw(&disk.file_amount_block, sizeof(fds_disk_file_amount_block));
    block_idx++;

    for (int i = 0; i < disk.files_count; ++i)
    {
        disk.file_header_blocks[i] = read_file_header_block(&file_ctx);
        disk.file_data_blocks[i] = read_file_data_block(&file_ctx, disk.file_header_blocks[i].file_size);
        //free(file_data_block.disk_data);

        raw_data_blocks[block_idx] = create_fds_block_raw(&disk.file_header_blocks[i], sizeof(fds_disk_file_header_block));
        block_idx++;

        raw_data_blocks[block_idx] = create_fds_block_raw(&disk.file_header_blocks[i], sizeof(fds_disk_file_header_block));
        block_idx++;
    }

    disk.raw_blocks = raw_data_blocks;

    // Read Outer Files
    // TODO

    sd_card_close_file(file_ctx);

    return disk;
}

int main() 
{
    stdio_init_all();
    
    printf("start fds");

    fds_state fds_state;
    fds_state.ram_adapter = create_fds_ram_adapter();
    
    fds_state_reset(&fds_state);

    fds_state.disk = read_rom_from_sd();
    fds_state.current = fds_disk_set;

    led_set_active(true);

    while (true)
    {
        if (!fds_state_update(&fds_state))
        {
            led_show_error();
        }
    }
}

/*
int main() 
{
    stdio_init_all();
    time_init();

    init_pins();
    init_fds();
    
    gpio_put(LED_PIN, 0);

    led_delay = to_us_since_boot(make_timeout_time_ms(LED_DELAY));

    //wait_for_button_down();
    
    read_disk_side(0, buffer);

    gpio_put(LED_PIN, 1);

    while (true) 
    {
        update_current_time();

        fds_update();

        led_blink_handler();
    }
}
*/