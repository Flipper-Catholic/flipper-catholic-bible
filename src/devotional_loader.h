#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEVOTIONAL_MAGIC 0x4F564544  /* "DEVO" LE */
#define DEVOTIONAL_MAX_TITLE  256
#define DEVOTIONAL_MAX_TEXT  1024

typedef struct DevotionalLoader DevotionalLoader;

struct DevotionalLoader {
    bool loaded;
    uint8_t* data;
    size_t size;
    uint16_t num_prayers;
};

/* Initialize from path to devotional.bin. Returns true if loaded. */
bool devotional_loader_init(DevotionalLoader* loader, const char* path);

void devotional_loader_free(DevotionalLoader* loader);

uint16_t devotional_loader_prayer_count(const DevotionalLoader* loader);

/* Get prayer by index (0-based). Copies title and text into buffers. Returns true on success. */
bool devotional_loader_get_prayer(
    const DevotionalLoader* loader,
    uint16_t index,
    char* title_buf,
    size_t title_size,
    char* text_buf,
    size_t text_size
);
