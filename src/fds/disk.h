#include "..\types.h"
#include "..\storage\sd_card.h"

#define DISK_SIDE_SIZE 65500

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
    byte unknown_0x1A[5];
    byte manufacturing_date[3];
    byte country_code;
    byte unknown_0x23;
    byte unknown_0x24;
    byte unknown_0x25[2];
    byte unknown_0x27[5];
    byte rewrite_disk_date[3];
    byte unknown_0x2F;
    byte unknown_0x30;
    byte disk_writer_serial_number[2];
    byte unknown_0x33;
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
    ushort size; // 0x04
    byte *disk_data;

    ushort crc;

} fds_disk_file_data_block;

typedef struct
{
    uint size;
    byte *data;
} fds_block_raw;

typedef struct 
{
    fds_disk_info_block         info_block;
    fds_disk_file_amount_block  file_amount_block;
    uint files_count;
    fds_disk_file_header_block  *file_header_blocks;
    fds_disk_file_data_block    *file_data_blocks;

    uint raw_blocks_count;
    fds_block_raw *raw_blocks;

} fds_disk;

ushort get_fds_crc(byte* data, uint size);

fds_disk_info_block read_info_block(sd_card_file_ctx* file_ctx);
fds_disk_file_amount_block read_file_amount_block(sd_card_file_ctx* file_ctx);
fds_disk_file_header_block read_file_header_block(sd_card_file_ctx* file_ctx);
fds_disk_file_data_block read_file_data_block(sd_card_file_ctx* file_ctx, uint block_size);

void read_disk_data_from_sd(sd_card_file_ctx file_ctx);