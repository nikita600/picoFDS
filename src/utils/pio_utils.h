#include "pico/stdlib.h"
#include "hardware/pio.h"

typedef struct
{
    PIO pio;
    uint sm;
} pio_ctx;

static inline void set_active_pio_ctx(pio_ctx *ctx, bool active)
{
    pio_sm_set_enabled(ctx->pio, ctx->sm, active);
}