#include "devotional_loader.h"
#include <furi.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/file_stream.h>
#include <string.h>

bool devotional_loader_init(DevotionalLoader* loader, const char* path) {
    if(!loader || !path) return false;
    memset(loader, 0, sizeof(DevotionalLoader));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) return false;
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    if(!file_stream_open(stream, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    size_t size = stream_size(stream);
    if(size < 8) {
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    uint8_t* data = malloc(size);
    if(!data) {
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    if(stream_read(stream, data, size) != size) {
        free(data);
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    uint32_t magic = *(uint32_t*)data;
    uint16_t version = *(uint16_t*)(data + 4);
    if(magic != DEVOTIONAL_MAGIC || version != 1) {
        free(data);
        return false;
    }
    loader->data = data;
    loader->size = size;
    loader->num_prayers = *(uint16_t*)(data + 6);
    loader->loaded = true;
    return true;
}

void devotional_loader_free(DevotionalLoader* loader) {
    if(!loader) return;
    if(loader->data) {
        free(loader->data);
        loader->data = NULL;
    }
    loader->size = 0;
    loader->num_prayers = 0;
    loader->loaded = false;
}

uint16_t devotional_loader_prayer_count(const DevotionalLoader* loader) {
    return loader && loader->loaded ? loader->num_prayers : 0;
}

bool devotional_loader_get_prayer(
    const DevotionalLoader* loader,
    uint16_t index,
    char* title_buf,
    size_t title_size,
    char* text_buf,
    size_t text_size
) {
    if(!loader || !loader->loaded || !title_buf || !text_buf || title_size == 0 || text_size == 0)
        return false;
    if(index >= loader->num_prayers) return false;
    const uint8_t* p = loader->data + 8;
    for(uint16_t i = 0; i < index; i++) {
        uint16_t tl = *(uint16_t*)p;
        p += 2 + tl;
        uint16_t xl = *(uint16_t*)p;
        p += 2 + xl;
        if(p >= loader->data + loader->size) return false;
    }
    uint16_t tl = *(uint16_t*)p;
    p += 2;
    if(p + tl > loader->data + loader->size) return false;
    size_t copy_title = (tl >= title_size) ? title_size - 1 : tl;
    memcpy(title_buf, p, copy_title);
    title_buf[copy_title] = '\0';
    p += tl;
    uint16_t xl = *(uint16_t*)p;
    p += 2;
    if(p + xl > loader->data + loader->size) return false;
    size_t copy_text = (xl >= text_size) ? text_size - 1 : xl;
    memcpy(text_buf, p, copy_text);
    text_buf[copy_text] = '\0';
    return true;
}
