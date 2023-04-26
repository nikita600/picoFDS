// Base clock 125Mhz
// NOP for base clock is 8ns

static inline void wait_for_0_1ms()
{
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");

    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");

    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
}

static inline void wait_for_1ms()
{
    wait_for_0_1ms();
    wait_for_0_1ms();
    wait_for_0_1ms();
    wait_for_0_1ms();
    wait_for_0_1ms();
    
    wait_for_0_1ms();
    wait_for_0_1ms();
    wait_for_0_1ms();
    wait_for_0_1ms();
    wait_for_0_1ms();
}

static inline void fds_wait_for_reset()
{
    wait_for_1ms();
}

// sleeps for 20746 ns(p) 
static inline void fds_wait_for_sample()
{
    wait_for_1ms();
    wait_for_1ms();
    wait_for_1ms();
    wait_for_1ms();
    //wait_for_1ms();

    //wait_for_0_1ms();
    //wait_for_0_1ms();
}