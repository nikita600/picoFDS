#include <string.h>

#include "stdlib.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "disk.h"

ushort get_fds_crc(byte* data, uint size)
{
  //Do not include any existing checksum, not even the blank checksums 00 00 or FF FF.
  //The formula will automatically count 2 0x00 bytes without the programmer adding them manually.
  //Also, do not include the gap terminator (0x80) in the data.
  //If you wish to do so, change sum to 0x0000.
  ushort sum = 0x8000;

  for(uint byte_index = 0; byte_index < size + 2; byte_index++)
{
    byte byte = byte_index < size ? data[byte_index] : 0x00;
    for(uint bit_index = 0; bit_index < 8; bit_index++) 
    {
      bool bit = (byte >> bit_index) & 1;
      bool carry = sum & 1;
      sum = (sum >> 1) | (bit << 15);
      if(carry) 
      {
        sum ^= 0x8408;
      }
    }
  }
  return sum;
}

fds_disk_info_block read_info_block(sd_card_file_ctx* file_ctx)
{
    uint block_size = sizeof(fds_disk_info_block) - sizeof(ushort);

    fds_disk_info_block disk_info_block;
    sd_card_read_bytes(file_ctx, &disk_info_block, block_size);
    if (disk_info_block.block_code != 0x01)
    {
        panic("fds_disk::read - fail to read fds_disk_info_block");
    }
    
    disk_info_block.crc = get_fds_crc((byte*)&disk_info_block, (uint)block_size);
    return disk_info_block;
}

fds_disk_file_amount_block read_file_amount_block(sd_card_file_ctx* file_ctx)
{
    uint block_size = sizeof(fds_disk_file_amount_block) - sizeof(ushort);

    fds_disk_file_amount_block file_amount_block;
    sd_card_read_bytes(file_ctx, &file_amount_block, block_size);
    if (file_amount_block.block_code != 0x02)
    {
        panic("fds_disk::read - fail to read fds_disk_file_amount_block");
    }

    file_amount_block.crc = get_fds_crc((byte*)&file_amount_block, (uint)block_size);
    return file_amount_block;
}

fds_disk_file_header_block read_file_header_block(sd_card_file_ctx* file_ctx)
{
    uint block_size = sizeof(fds_disk_file_header_block) - sizeof(ushort);

    fds_disk_file_header_block file_header_block;
    sd_card_read_bytes(file_ctx, &file_header_block, block_size);
    if (file_header_block.block_code != 0x03)
    {
        panic("fds_disk::read - fail to read fds_disk_file_header_block");
    }

    file_header_block.crc = get_fds_crc((byte*)&file_header_block, (uint)block_size);
    return file_header_block;
}

fds_disk_file_data_block read_file_data_block(sd_card_file_ctx* file_ctx, uint block_size)
{
    fds_disk_file_data_block file_data_block;
    file_data_block.size = block_size + 1;
    file_data_block.disk_data = (byte*)malloc(file_data_block.size);
    sd_card_read_bytes(file_ctx, file_data_block.disk_data, file_data_block.size);
    if (file_data_block.disk_data[0] != 0x04)
    {
        panic("fds_disk::read - fail to read fds_disk_file_data_block");
    }
    
    file_data_block.crc = get_fds_crc(file_data_block.disk_data, file_data_block.size);
    return file_data_block;
}

/*
void read_disk_data(void* src_buffer)
{
    byte* buffer = src_buffer;

    fds_disk_info_block disk_info_block = read_info_block(&buffer);
    fds_disk_file_amount_block file_amount_block = read_file_amount_block(&buffer);

    for (int i = 0; i < file_amount_block.file_amount; ++i)
    {
        fds_disk_file_header_block file_header_block = read_file_header_block(&buffer);
        fds_disk_file_data_block file_data_block = read_file_data_block(&buffer, file_header_block.file_size);
        free(file_data_block.disk_data);
    }
}
*/