#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Storage Adapter for Catholic Bible App
 * Handles SD card file access for Bible text assets
 * Phase 2.2: SD Card Storage Adapter
 */

#define STORAGE_BASE_PATH "/apps_data/bible"
#define STORAGE_BIBLE_TEXT STORAGE_BASE_PATH "/bible_text.bin"
#define STORAGE_VERSE_INDEX STORAGE_BASE_PATH "/verse_index.bin"
#define STORAGE_CANON_TABLE STORAGE_BASE_PATH "/canon_table.bin"
#define STORAGE_METADATA STORAGE_BASE_PATH "/metadata.json"

/* Verse Index File Format (per data-index-layout.md) */
#define VERSE_INDEX_MAGIC 0x56494458  // "VIDX"
#define VERSE_INDEX_VERSION 1

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;        // VIDX
    uint8_t version;       // 1
    uint32_t total_verses;
} VerseIndexHeader;

typedef struct {
    uint32_t text_offset;  // Offset in bible_text.bin
    uint16_t text_len;     // Length of verse text
    uint8_t book_id;       // Book index (0-72)
    uint16_t chapter;      // Chapter number (1-based)
    uint16_t verse;        // Verse number (1-based)
    uint8_t reserved;      // Padding
} VerseIndexRecord;
#pragma pack(pop)

/* Storage Adapter State */
typedef struct {
    bool initialized;
    bool sd_card_present;
    bool assets_available;
    bool use_bundled_assets;  /* true = read from FAP bundle (/assets), false = /apps_data/bible */

    /* Paths to use (set by init: either SD or APP_ASSETS_PATH) */
    const char* path_bible_text;
    const char* path_verse_index;

    // File streams (opened on demand)
    void* bible_text_stream;  // Stream* handle (opaque to avoid include)
    void* verse_index_stream; // Stream* handle (opaque to avoid include)

    // Cached index data
    uint32_t total_verses;
    VerseIndexRecord* index_cache;  // NULL if not loaded
    size_t index_cache_size;

    // Error state
    char last_error[128];
} StorageAdapter;

/* Initialize storage adapter
 * Returns true if SD card is present and assets directory exists
 * Does not validate asset files (use storage_adapter_validate_assets)
 */
bool storage_adapter_init(StorageAdapter* adapter);

/* Cleanup storage adapter resources */
void storage_adapter_free(StorageAdapter* adapter);

/* Check if SD card is present */
bool storage_adapter_is_sd_present(StorageAdapter* adapter);

/* Validate that required asset files exist
 * Returns true if all required files are present
 * Sets last_error on failure
 */
bool storage_adapter_validate_assets(StorageAdapter* adapter);

/* Load verse index into memory
 * Returns true on success, false on error (check last_error)
 */
bool storage_adapter_load_index(StorageAdapter* adapter);

/* Get verse text from SD card
 * book_index: 0-72 (Catholic canon)
 * chapter: 1-based chapter number
 * verse: 1-based verse number
 * buffer: output buffer for verse text
 * buffer_size: size of buffer
 * 
 * Returns: number of bytes written (excluding null terminator)
 * Returns 0 on error (check last_error)
 * 
 * Note: Falls back to hardcoded text if SD card not available
 */
size_t storage_adapter_get_verse_text(
    StorageAdapter* adapter,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse,
    char* buffer,
    size_t buffer_size
);

/* Get verse count for a chapter
 * Returns 0 on error or if not found
 */
uint16_t storage_adapter_get_verse_count(
    StorageAdapter* adapter,
    size_t book_index,
    uint16_t chapter
);

/* Get (book_id, chapter, verse) from verse_id (0-based index in canonical order).
 * Returns true on success. Loads index if needed. */
bool storage_adapter_get_ref_from_verse_id(
    StorageAdapter* adapter,
    uint32_t verse_id,
    uint8_t* book_id,
    uint16_t* chapter,
    uint16_t* verse
);

/* Get last error message */
const char* storage_adapter_get_error(StorageAdapter* adapter);

/* Check if assets are available (SD card present and files exist) */
bool storage_adapter_assets_available(StorageAdapter* adapter);
