#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MISSAL_MAGIC 0x5353494D
#define MISSAL_MAX_TEXT 2048
#define MISSAL_MAX_TITLE 128

typedef struct MissalLoader MissalLoader;

struct MissalLoader {
    bool loaded;
    uint8_t* data;
    size_t size;
    uint16_t num_seasons;
    uint16_t num_mass_prayers;
    uint16_t num_mass_responses;
    uint16_t num_readings;
};

bool missal_loader_init(MissalLoader* loader, const char* path);
void missal_loader_free(MissalLoader* loader);

/* Seasons: 0..num_seasons-1. title_buf/desc_buf optional (NULL to skip). */
bool missal_loader_get_season(const MissalLoader* l, uint16_t idx, char* title_buf, size_t title_sz, char* desc_buf, size_t desc_sz);

/* Mass prayers: 0..num_mass_prayers-1. */
bool missal_loader_get_mass_prayer(const MissalLoader* l, uint16_t idx, char* title_buf, size_t title_sz, char* text_buf, size_t text_sz);

/* Mass responses: 0..num_mass_responses-1. */
bool missal_loader_get_mass_response(const MissalLoader* l, uint16_t idx, char* title_buf, size_t title_sz, char* text_buf, size_t text_sz);

/* Readings: 0..num_readings-1. key_buf optional. Puts combined text in text_buf if not NULL. */
bool missal_loader_get_reading(const MissalLoader* l, uint16_t idx, char* key_buf, size_t key_sz, char* title_buf, size_t title_sz, char* text_buf, size_t text_sz);
