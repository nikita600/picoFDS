#include "pico/stdlib.h"
#include "hardware/pio.h"

typedef	unsigned char byte;

typedef struct __attribute__((packed))
{
    byte block_code; // 0x01
    byte disk_verification[14];
    byte licensee_code;
    byte game_name[3];
    byte game_type;
    byte game_version;
    byte side_number;
    byte disk_number;
    byte disk_type;
    byte unknown_0x18;
    byte boot_read_file_code;
    byte unknown0x1A[5];
    byte manufacturing_date[3];
    byte country_code;
    byte unknown0x23;
    byte unknown0x24;
    byte unknown0x25[2];
    byte unknown0x27[5];
    byte rewrite_disk_date[3];
    byte unknown0x2F;
    byte unknown0x30;
    byte disk_writer_serial_number[2];
    byte unknown0x33;
    byte disk_rewrite_count;
    byte actual_disk_side;
    byte disk_type_other;
    byte disk_version;

    ushort crc;

} fds_disk_info_block;

typedef struct __attribute__((packed))
{
    byte block_code; // 0x02
    byte file_amount;

    ushort crc;

} fds_disk_file_amount_block;

typedef struct __attribute__((packed))
{
    byte block_code; // 0x03
    byte file_number;
    byte file_id;
    byte file_name[8];
    ushort file_address;
    ushort file_size;
    byte file_type;

    ushort crc;

} fds_disk_file_header_block;

typedef struct __attribute__((packed))
{
    byte block_code; // 0x04
    byte *disk_data;

    ushort crc;

} fds_disk_file_data_block;

typedef struct 
{
    fds_disk_info_block         info_block;
    fds_disk_file_amount_block  file_amount_block;
    fds_disk_file_header_block  *file_header_blocks;
    fds_disk_file_data_block    *file_data_blocks;

} fds_disk;

static inline void read_disk_data(void* buffer)
{
    uint size00 = sizeof(ushort);
    uint size01 = sizeof(fds_disk_info_block);
    uint size02 = sizeof(fds_disk_file_amount_block);
    uint size03 = sizeof(fds_disk_file_header_block);
    uint size04 = sizeof(fds_disk_file_data_block);

    byte* buffer_position = buffer;

    fds_disk_info_block disk_info_block;
    memcpy(&disk_info_block, buffer_position, sizeof(fds_disk_info_block) - size00);
    buffer_position += sizeof(fds_disk_info_block) - size00;
    if (disk_info_block.block_code != 0x01)
    {
        panic("fds_disk::read - fail to read fds_disk_info_block");
    }

    fds_disk_file_amount_block file_amount_block;
    memcpy(&file_amount_block, buffer_position, (sizeof(fds_disk_file_amount_block) - size00));
    buffer_position += sizeof(fds_disk_file_amount_block) - size00;
    if (file_amount_block.block_code != 0x02)
    {
        panic("fds_disk::read - fail to read fds_disk_file_amount_block");
    }

    for (int i = 0; i < file_amount_block.file_amount; ++i)
    {
        fds_disk_file_header_block file_header_block;
        memcpy(&file_header_block, buffer_position, (sizeof(fds_disk_file_header_block) - size00));
        buffer_position += sizeof(fds_disk_file_header_block) - size00;
        if (file_header_block.block_code != 0x03)
        {
            panic("fds_disk::read - fail to read fds_disk_file_header_block");
        }

        fds_disk_file_data_block file_data_block;
        memcpy(&file_data_block, buffer_position, 1);
        buffer_position += file_header_block.file_size + 1;
        if (file_data_block.block_code != 0x04)
        {
            panic("fds_disk::read - fail to read fds_disk_file_data_block");
        }
    }

}