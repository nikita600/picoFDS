#ifndef FDS_H_
#define FDS_H_

// Constants
#define DISK_SIDE_SIZE 65500

#define TRANSFER_FREQ_HZ 96400 / 2

void read_disk_side(int side, const void* buffer);

void wait_for_button_down();

#endif