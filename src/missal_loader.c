#include "missal_loader.h"
#include <furi.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/file_stream.h>
#include <string.h>

static bool read_str(const uint8_t** p, const uint8_t* end, char* buf, size_t buf_sz) {
    if(*p + 2 > end) return false;
    uint16_t len = *(uint16_t*)(*p);
    *p += 2;
    if(*p + len > end) return false;
    if(buf && buf_sz) {
        size_t copy = len >= buf_sz ? buf_sz - 1 : len;
        memcpy(buf, *p, copy);
        buf[copy] = '\0';
    }
    *p += len;
    return true;
}

static void skip_n_str(const uint8_t** p, const uint8_t* end, uint16_t n) {
    for(uint16_t i = 0; i < n && *p + 2 <= end; i++) {
        uint16_t len = *(uint16_t*)(*p);
        *p += 2 + len;
    }
}

bool missal_loader_init(MissalLoader* loader, const char* path) {
    if(!loader || !path) return false;
    memset(loader, 0, sizeof(MissalLoader));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) return false;
    Stream* stream = file_stream_alloc(storage);
    if(!stream) { furi_record_close(RECORD_STORAGE); return false; }
    if(!file_stream_open(stream, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_free(stream); furi_record_close(RECORD_STORAGE); return false;
    }
    size_t size = 0;
    if(!stream_size(stream, &size) || size < 12) {
        file_stream_close(stream); stream_free(stream); furi_record_close(RECORD_STORAGE); return false;
    }
    uint8_t* data = malloc(size);
    if(!data) { file_stream_close(stream); stream_free(stream); furi_record_close(RECORD_STORAGE); return false; }
    if(stream_read(stream, data, size) != size) {
        free(data); file_stream_close(stream); stream_free(stream); furi_record_close(RECORD_STORAGE); return false;
    }
    file_stream_close(stream); stream_free(stream); furi_record_close(RECORD_STORAGE);
    if(*(uint32_t*)data != MISSAL_MAGIC || *(uint16_t*)(data + 4) != 1) { free(data); return false; }
    const uint8_t* p = data + 6;
    const uint8_t* end = data + size;
    loader->num_seasons = *(uint16_t*)p; p += 2;
    for(uint16_t i = 0; i < loader->num_seasons && p + 2 <= end; i++) {
        uint16_t L = *(uint16_t*)p; p += 2 + L;
        if(p + 2 <= end) { L = *(uint16_t*)p; p += 2 + L; }
    }
    loader->num_mass_prayers = p + 2 <= end ? *(uint16_t*)p : 0; p += 2;
    for(uint16_t i = 0; i < loader->num_mass_prayers && p + 2 <= end; i++) {
        uint16_t L = *(uint16_t*)p; p += 2 + L;
        if(p + 2 <= end) { L = *(uint16_t*)p; p += 2 + L; }
    }
    loader->num_mass_responses = p + 2 <= end ? *(uint16_t*)p : 0; p += 2;
    for(uint16_t i = 0; i < loader->num_mass_responses && p + 2 <= end; i++) {
        uint16_t L = *(uint16_t*)p; p += 2 + L;
        if(p + 2 <= end) { L = *(uint16_t*)p; p += 2 + L; }
    }
    loader->num_readings = p + 2 <= end ? *(uint16_t*)p : 0;
    loader->data = data;
    loader->size = size;
    loader->loaded = true;
    return true;
}

void missal_loader_free(MissalLoader* loader) {
    if(!loader) return;
    if(loader->data) free(loader->data);
    loader->data = NULL; loader->size = 0; loader->loaded = false;
    loader->num_seasons = loader->num_mass_prayers = loader->num_mass_responses = loader->num_readings = 0;
}

bool missal_loader_get_season(const MissalLoader* l, uint16_t idx, char* title_buf, size_t title_sz, char* desc_buf, size_t desc_sz) {
    if(!l || !l->loaded || idx >= l->num_seasons) return false;
    const uint8_t* p = l->data + 8;
    const uint8_t* end = l->data + l->size;
    skip_n_str(&p, end, idx * 2);
    return read_str(&p, end, title_buf, title_sz) && read_str(&p, end, desc_buf, desc_sz);
}

bool missal_loader_get_mass_prayer(const MissalLoader* l, uint16_t idx, char* title_buf, size_t title_sz, char* text_buf, size_t text_sz) {
    if(!l || !l->loaded || idx >= l->num_mass_prayers) return false;
    const uint8_t* p = l->data + 8;
    const uint8_t* end = l->data + l->size;
    skip_n_str(&p, end, l->num_seasons * 2);
    if(p + 2 > end) return false;
    p += 2;
    skip_n_str(&p, end, idx * 2);
    return read_str(&p, end, title_buf, title_sz) && read_str(&p, end, text_buf, text_sz);
}

bool missal_loader_get_mass_response(const MissalLoader* l, uint16_t idx, char* title_buf, size_t title_sz, char* text_buf, size_t text_sz) {
    if(!l || !l->loaded || idx >= l->num_mass_responses) return false;
    const uint8_t* p = l->data + 8;
    const uint8_t* end = l->data + l->size;
    skip_n_str(&p, end, l->num_seasons * 2);
    p += 2;
    skip_n_str(&p, end, l->num_mass_prayers * 2);
    if(p + 2 > end) return false;
    p += 2;
    skip_n_str(&p, end, idx * 2);
    return read_str(&p, end, title_buf, title_sz) && read_str(&p, end, text_buf, text_sz);
}

bool missal_loader_get_reading(const MissalLoader* l, uint16_t idx, char* key_buf, size_t key_sz, char* title_buf, size_t title_sz, char* text_buf, size_t text_sz) {
    if(!l || !l->loaded || idx >= l->num_readings) return false;
    const uint8_t* p = l->data + 8;
    const uint8_t* end = l->data + l->size;
    skip_n_str(&p, end, l->num_seasons * 2);
    p += 2;
    skip_n_str(&p, end, l->num_mass_prayers * 2);
    p += 2;
    skip_n_str(&p, end, l->num_mass_responses * 2);
    if(p + 2 > end) return false;
    p += 2;
    for(uint16_t i = 0; i < idx; i++) {
        if(p + 1 > end) return false;
        uint8_t kl = *p++; if(p + kl > end) return false;
        p += kl;
        if(!read_str(&p, end, NULL, 0) || !read_str(&p, end, NULL, 0) || !read_str(&p, end, NULL, 0) || !read_str(&p, end, NULL, 0)) return false;
    }
    if(p + 1 > end) return false;
    uint8_t kl = *p++;
    if(p + kl > end) return false;
    if(key_buf && key_sz) { size_t c = kl >= key_sz ? key_sz - 1 : kl; memcpy(key_buf, p, c); key_buf[c] = '\0'; }
    p += kl;
    char title[MISSAL_MAX_TITLE], fr[MISSAL_MAX_TEXT], ps[MISSAL_MAX_TEXT], g[MISSAL_MAX_TEXT];
    if(!read_str(&p, end, title, sizeof(title))) return false;
    if(!read_str(&p, end, fr, sizeof(fr))) return false;
    if(!read_str(&p, end, ps, sizeof(ps))) return false;
    if(!read_str(&p, end, g, sizeof(g))) return false;
    if(title_buf && title_sz) { strncpy(title_buf, title, title_sz - 1); title_buf[title_sz - 1] = '\0'; }
    if(text_buf && text_sz) snprintf(text_buf, text_sz, "%s\n\n%s\n\n%s", fr, ps, g);
    return true;
}
