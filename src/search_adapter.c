/**
 * Search adapter: sharded inverted index lookup (Phase 3).
 * Reads search_shard_map.bin and search_shards/shard_*.bin produced by build_search_index.py.
 */

#include "search_adapter.h"
#include <furi.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/file_stream.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define SEARCH_MAGIC 0x53494458
#define SEARCH_VERSION 1
#define PREFIX_CHARS 26
#define MAX_TOKEN_LEN 32

static int prefix_index(const char* token) {
    if(!token[0] || !token[1]) return 0;
    char a = (char)tolower((unsigned char)token[0]);
    char b = (char)tolower((unsigned char)token[1]);
    if(!isalpha((unsigned char)a) || !isalpha((unsigned char)b)) return 0;
    return (a - 'a') * PREFIX_CHARS + (b - 'a');
}

static void normalize_query(const char* query, char* out, size_t out_size) {
    size_t j = 0;
    for(size_t i = 0; query[i] && j < out_size - 1; i++) {
        char c = (char)tolower((unsigned char)query[i]);
        if(isalpha((unsigned char)c)) {
            out[j++] = c;
            if(j >= MAX_TOKEN_LEN) break;
        } else if(j > 0)
            break; /* first word only */
    }
    out[j] = '\0';
}

bool search_adapter_init(SearchAdapter* adapter, const char* base_path) {
    if(!adapter || !base_path) return false;
    memset(adapter, 0, sizeof(SearchAdapter));
    snprintf(adapter->path_shard_map, sizeof(adapter->path_shard_map), "%s/search_shard_map.bin", base_path);
    snprintf(adapter->path_shards_dir, sizeof(adapter->path_shards_dir), "%s/search_shards", base_path);
    adapter->current_shard_id = -1;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) return false;
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    if(!file_stream_open(stream, adapter->path_shard_map, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    uint32_t magic;
    uint16_t version, count;
    if(stream_read(stream, (uint8_t*)&magic, 4) != 4 || magic != SEARCH_MAGIC ||
       stream_read(stream, (uint8_t*)&version, 2) != 2 || version != SEARCH_VERSION ||
       stream_read(stream, (uint8_t*)&count, 2) != 2 || count != SEARCH_SHARD_MAP_ENTRIES) {
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    for(int i = 0; i < SEARCH_SHARD_MAP_ENTRIES; i++) {
        if(stream_read(stream, (uint8_t*)&adapter->shard_map[i], 2) != 2) break;
    }
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    adapter->shard_map_loaded = true;
    adapter->initialized = true;
    return true;
}

void search_adapter_free(SearchAdapter* adapter) {
    if(!adapter) return;
    if(adapter->shard_data) {
        free(adapter->shard_data);
        adapter->shard_data = NULL;
        adapter->shard_size = 0;
    }
    adapter->current_shard_id = -1;
    adapter->initialized = false;
    adapter->shard_map_loaded = false;
}

static bool load_shard(SearchAdapter* adapter, int shard_id) {
    if(adapter->current_shard_id == shard_id && adapter->shard_data) return true;
    if(adapter->shard_data) {
        free(adapter->shard_data);
        adapter->shard_data = NULL;
        adapter->shard_size = 0;
        adapter->current_shard_id = -1;
    }
    char path[120];
    snprintf(path, sizeof(path), "%s/shard_%03d.bin", adapter->path_shards_dir, shard_id);
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
    size_t size = 0;
    if(!stream_size(stream, &size) || size == 0 || size > 512 * 1024) {
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
    adapter->shard_data = data;
    adapter->shard_size = size;
    adapter->current_shard_id = shard_id;
    return true;
}

/* Parse shard, find token (prefix match), fill verse_ids. Shard format: magic(4), ver(2), num_tokens(4), then per token: len(1), token[len], num_refs(2), refs[num_refs](4). */
static size_t find_in_shard(SearchAdapter* adapter, const char* token, uint32_t* verse_ids_out, size_t max_results) {
    if(!adapter->shard_data || !token) return 0;
    uint8_t* p = adapter->shard_data;
    uint8_t* end = p + adapter->shard_size;
    if(p + 4 + 2 + 4 > end) return 0;
    uint32_t magic = *(uint32_t*)p;
    p += 4;
    if(magic != SEARCH_MAGIC) return 0;
    p += 2; /* version */
    uint32_t num_tokens = *(uint32_t*)p;
    p += 4;
    size_t found = 0;
    size_t token_len = strlen(token);
    if(token_len < 2) return 0;

    for(uint32_t i = 0; i < num_tokens && p < end && found < max_results; i++) {
        uint8_t len = *p++;
        if(p + len + 2 > end) break;
        /* Prefix match: token is prefix of this dict token or equal */
        int cmp = 0;
        for(size_t k = 0; k < token_len && k < len; k++) {
            char c = (char)tolower((unsigned char)p[k]);
            if(c != token[k]) { cmp = c - token[k]; break; }
        }
        if(cmp > 0) break; /* past possible matches */
        if(cmp == 0 && len >= (uint8_t)token_len) {
            p += len;
            uint16_t num_refs = *(uint16_t*)p;
            p += 2;
            if(p + (size_t)num_refs * 4 > end) break;
            for(uint16_t r = 0; r < num_refs && found < max_results; r++) {
                verse_ids_out[found++] = *(uint32_t*)(p + r * 4);
            }
            p += (size_t)num_refs * 4;
        } else {
            p += len;
            uint16_t num_refs = *(uint16_t*)p;
            p += 2 + (size_t)num_refs * 4;
        }
    }
    return found;
}

size_t search_adapter_lookup(SearchAdapter* adapter, const char* query, uint32_t* verse_ids_out, size_t max_results) {
    if(!adapter || !adapter->shard_map_loaded || !query || !verse_ids_out || max_results == 0) return 0;
    char norm[SEARCH_MAX_QUERY_LEN];
    normalize_query(query, norm, sizeof(norm));
    if(strlen(norm) < 2) return 0;
    int pref = prefix_index(norm);
    uint16_t shard_id = adapter->shard_map[pref];
    if(shard_id == 0xFFFF) return 0;
    if(!load_shard(adapter, (int)shard_id)) return 0;
    return find_in_shard(adapter, norm, verse_ids_out, max_results);
}

bool search_adapter_available(SearchAdapter* adapter) {
    return adapter && adapter->initialized && adapter->shard_map_loaded;
}
