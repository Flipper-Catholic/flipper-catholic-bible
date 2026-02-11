#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Search adapter: loads sharded search index, lookup by token (prefix match).
 * Phase 3. Requires verse index in storage_adapter for verse_id -> ref. */

#define SEARCH_MAX_RESULTS 64
#define SEARCH_MAX_QUERY_LEN 32
#define SEARCH_SHARD_MAP_ENTRIES 676  /* 26*26 */

typedef struct {
    bool initialized;
    char path_shard_map[96];
    char path_shards_dir[96];
    uint16_t shard_map[SEARCH_SHARD_MAP_ENTRIES];  /* prefix index -> shard file index */
    bool shard_map_loaded;
    /* One shard in memory at a time */
    uint8_t* shard_data;
    size_t shard_size;
    int current_shard_id;  /* -1 if none loaded */
} SearchAdapter;

/* Initialize. base_path = directory containing search_shard_map.bin and search_shards/ */
bool search_adapter_init(SearchAdapter* adapter, const char* base_path);

void search_adapter_free(SearchAdapter* adapter);

/* Lookup: normalize query to first token (lowercase, alpha only), find matching verses.
 * verse_ids_out: filled with up to max_results verse_ids (0-based canonical).
 * Returns number of results. 0 = no index, no match, or error. */
size_t search_adapter_lookup(
    SearchAdapter* adapter,
    const char* query,
    uint32_t* verse_ids_out,
    size_t max_results
);

/* Check if search index is available (shard map loaded). */
bool search_adapter_available(SearchAdapter* adapter);
