#include "storage_adapter.h"

#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/file_stream.h>
#include <string.h>
#include <stdio.h>

/* Forward declarations */
static bool storage_adapter_check_sd_card(StorageAdapter* adapter);
static bool storage_adapter_check_assets_exist(StorageAdapter* adapter);
static bool storage_adapter_read_verse_index_header(StorageAdapter* adapter);
static bool storage_adapter_find_verse_in_index(
    StorageAdapter* adapter,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse,
    uint32_t* out_offset,
    uint16_t* out_length
);

/* Initialize storage adapter */
bool storage_adapter_init(StorageAdapter* adapter) {
    if(!adapter) return false;
    
    memset(adapter, 0, sizeof(StorageAdapter));
    adapter->initialized = true;
    
    // Check SD card presence (for /apps_data/bible path)
    adapter->sd_card_present = storage_adapter_check_sd_card(adapter);
    
    // Prefer SD /apps_data/bible; fall back to bundled FAP assets (works even without SD)
    adapter->assets_available = storage_adapter_check_assets_exist(adapter);
    if(adapter->assets_available) {
        storage_adapter_read_verse_index_header(adapter);
    }
    
    return adapter->initialized;
}

/* Cleanup storage adapter */
void storage_adapter_free(StorageAdapter* adapter) {
    if(!adapter || !adapter->initialized) return;
    
    // Close file streams if open
    if(adapter->bible_text_stream) {
        stream_free(adapter->bible_text_stream);
        adapter->bible_text_stream = NULL;
    }
    
    if(adapter->verse_index_stream) {
        stream_free(adapter->verse_index_stream);
        adapter->verse_index_stream = NULL;
    }
    
    // Free index cache if loaded
    if(adapter->index_cache) {
        free(adapter->index_cache);
        adapter->index_cache = NULL;
        adapter->index_cache_size = 0;
    }
    
    adapter->initialized = false;
    adapter->assets_available = false;
}

/* Check if SD card is present */
bool storage_adapter_is_sd_present(StorageAdapter* adapter) {
    if(!adapter || !adapter->initialized) return false;
    return adapter->sd_card_present;
}

/* Validate that required asset files exist */
bool storage_adapter_validate_assets(StorageAdapter* adapter) {
    if(!adapter || !adapter->initialized) {
        if(adapter) {
            strncpy(adapter->last_error, "Adapter not initialized", sizeof(adapter->last_error) - 1);
        }
        return false;
    }
    
    if(!adapter->sd_card_present) {
        strncpy(adapter->last_error, "SD card not present", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    return storage_adapter_check_assets_exist(adapter);
}

/* Load verse index into memory */
bool storage_adapter_load_index(StorageAdapter* adapter) {
    if(!adapter || !adapter->initialized || !adapter->assets_available) {
        if(adapter) {
            strncpy(adapter->last_error, "Assets not available", sizeof(adapter->last_error) - 1);
        }
        return false;
    }
    
    // If already loaded, return success
    if(adapter->index_cache) {
        return true;
    }
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        strncpy(adapter->last_error, "Failed to open storage", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to allocate stream", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    // Open verse index file (SD path or bundled assets path)
    if(!file_stream_open(stream, adapter->path_verse_index, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to open verse_index.bin", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    // Read header
    VerseIndexHeader header;
    size_t bytes_read = stream_read(stream, (uint8_t*)&header, sizeof(header));
    if(bytes_read != sizeof(header)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to read index header", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    // Validate magic and version
    if(header.magic != VERSE_INDEX_MAGIC) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Invalid index magic number", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    if(header.version != VERSE_INDEX_VERSION) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        snprintf(adapter->last_error, sizeof(adapter->last_error), 
                 "Unsupported index version: %u (expected %d)", 
                 header.version, VERSE_INDEX_VERSION);
        return false;
    }
    
    adapter->total_verses = header.total_verses;
    
    // Allocate cache
    size_t cache_size = header.total_verses * sizeof(VerseIndexRecord);
    adapter->index_cache = malloc(cache_size);
    if(!adapter->index_cache) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to allocate index cache", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    // Read all index records
    bytes_read = stream_read(stream, (uint8_t*)adapter->index_cache, cache_size);
    if(bytes_read != cache_size) {
        free(adapter->index_cache);
        adapter->index_cache = NULL;
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to read index records", sizeof(adapter->last_error) - 1);
        return false;
    }
    
    adapter->index_cache_size = header.total_verses;
    
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    
    return true;
}

/* Get verse text from SD card */
size_t storage_adapter_get_verse_text(
    StorageAdapter* adapter,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse,
    char* buffer,
    size_t buffer_size
) {
    if(!adapter || !buffer || buffer_size == 0) {
        if(adapter) {
            strncpy(adapter->last_error, "Invalid parameters", sizeof(adapter->last_error) - 1);
        }
        return 0;
    }
    
    // If assets not available, return 0 (caller should fall back to hardcoded)
    if(!adapter->assets_available) {
        return 0;
    }
    
    // Find verse in index
    uint32_t text_offset = 0;
    uint16_t text_length = 0;
    
    if(!storage_adapter_find_verse_in_index(adapter, book_index, chapter, verse, 
                                            &text_offset, &text_length)) {
        strncpy(adapter->last_error, "Verse not found in index", sizeof(adapter->last_error) - 1);
        return 0;
    }
    
    // Ensure we don't overflow buffer
    if(text_length >= buffer_size) {
        text_length = buffer_size - 1;
    }
    
    // Open bible_text.bin and read verse
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        strncpy(adapter->last_error, "Failed to open storage", sizeof(adapter->last_error) - 1);
        return 0;
    }
    
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to allocate stream", sizeof(adapter->last_error) - 1);
        return 0;
    }
    
    if(!file_stream_open(stream, adapter->path_bible_text, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to open bible_text.bin", sizeof(adapter->last_error) - 1);
        return 0;
    }
    
    // Seek to verse offset
    if(!stream_seek(stream, text_offset, StreamOffsetFromStart)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to seek to verse", sizeof(adapter->last_error) - 1);
        return 0;
    }
    
    // Read verse text
    size_t bytes_read = stream_read(stream, (uint8_t*)buffer, text_length);
    if(bytes_read != text_length) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        strncpy(adapter->last_error, "Failed to read verse text", sizeof(adapter->last_error) - 1);
        return 0;
    }
    
    buffer[bytes_read] = '\0'; // Null terminate
    
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    
    return bytes_read;
}

/* Get verse count for a chapter */
uint16_t storage_adapter_get_verse_count(
    StorageAdapter* adapter,
    size_t book_index,
    uint16_t chapter
) {
    if(!adapter || !adapter->assets_available) {
        return 0;
    }
    
    // Load index if not already loaded
    if(!adapter->index_cache) {
        if(!storage_adapter_load_index(adapter)) {
            return 0;
        }
    }
    
    // Count verses in this chapter
    uint16_t count = 0;
    for(size_t i = 0; i < adapter->index_cache_size; i++) {
        VerseIndexRecord* record = &adapter->index_cache[i];
        if(record->book_id == book_index && record->chapter == chapter) {
            count++;
        }
    }
    
    return count;
}

/* Get last error message */
const char* storage_adapter_get_error(StorageAdapter* adapter) {
    if(!adapter) return "Adapter is NULL";
    return adapter->last_error;
}

/* Check if assets are available */
bool storage_adapter_assets_available(StorageAdapter* adapter) {
    if(!adapter || !adapter->initialized) return false;
    return adapter->assets_available;
}

/* Internal helper: Check SD card presence */
static bool storage_adapter_check_sd_card(StorageAdapter* adapter) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        if(adapter) {
            strncpy(adapter->last_error, "Failed to open storage", sizeof(adapter->last_error) - 1);
        }
        return false;
    }
    
    FS_Error error = storage_sd_status(storage);
    furi_record_close(RECORD_STORAGE);
    
    bool present = (error == FSE_OK);
    if(!present && adapter) {
        strncpy(adapter->last_error, "SD card not detected", sizeof(adapter->last_error) - 1);
    }
    
    return present;
}

/* Internal helper: Check if asset files exist (SD path or bundled FAP assets) */
static bool storage_adapter_check_assets_exist(StorageAdapter* adapter) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        if(adapter) {
            strncpy(adapter->last_error, "Failed to open storage", sizeof(adapter->last_error) - 1);
        }
        return false;
    }
    
    Stream* test_stream = file_stream_alloc(storage);
    if(!test_stream) {
        furi_record_close(RECORD_STORAGE);
        if(adapter) {
            snprintf(adapter->last_error, sizeof(adapter->last_error), "Failed to allocate test stream");
        }
        return false;
    }
    
    /* Try SD path first (/apps_data/bible/) */
    bool sd_ok = file_stream_open(test_stream, STORAGE_BIBLE_TEXT, FSAM_READ, FSOM_OPEN_EXISTING);
    if(sd_ok) {
        file_stream_close(test_stream);
        sd_ok = file_stream_open(test_stream, STORAGE_VERSE_INDEX, FSAM_READ, FSOM_OPEN_EXISTING);
        if(sd_ok) {
            file_stream_close(test_stream);
            adapter->path_bible_text = STORAGE_BIBLE_TEXT;
            adapter->path_verse_index = STORAGE_VERSE_INDEX;
            adapter->use_bundled_assets = false;
            stream_free(test_stream);
            furi_record_close(RECORD_STORAGE);
            return true;
        }
    }
    file_stream_close(test_stream);
    
    /* Fall back to bundled FAP assets (unpacked to /ext/apps_assets/... or /assets) */
    if(file_stream_open(test_stream, APP_ASSETS_PATH("bible_text.bin"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        file_stream_close(test_stream);
        if(file_stream_open(test_stream, APP_ASSETS_PATH("verse_index.bin"), FSAM_READ, FSOM_OPEN_EXISTING)) {
            file_stream_close(test_stream);
            adapter->path_bible_text = APP_ASSETS_PATH("bible_text.bin");
            adapter->path_verse_index = APP_ASSETS_PATH("verse_index.bin");
            adapter->use_bundled_assets = true;
            stream_free(test_stream);
            furi_record_close(RECORD_STORAGE);
            return true;
        }
    }
    file_stream_close(test_stream);
    
    stream_free(test_stream);
    furi_record_close(RECORD_STORAGE);
    if(adapter) {
        strncpy(adapter->last_error, "Bible assets not found (check SD or reinstall app)", sizeof(adapter->last_error) - 1);
    }
    return false;
}

/* Internal helper: Read verse index header */
static bool storage_adapter_read_verse_index_header(StorageAdapter* adapter) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) return false;
    
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    if(!file_stream_open(stream, adapter->path_verse_index, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    VerseIndexHeader header;
    size_t bytes_read = stream_read(stream, (uint8_t*)&header, sizeof(header));
    
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    
    if(bytes_read == sizeof(header) && header.magic == VERSE_INDEX_MAGIC) {
        adapter->total_verses = header.total_verses;
        return true;
    }
    
    return false;
}

/* Internal helper: Find verse in index */
static bool storage_adapter_find_verse_in_index(
    StorageAdapter* adapter,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse,
    uint32_t* out_offset,
    uint16_t* out_length
) {
    if(!adapter || !out_offset || !out_length) return false;
    
    // Load index if not already loaded
    if(!adapter->index_cache) {
        if(!storage_adapter_load_index(adapter)) {
            return false;
        }
    }
    
    // Search index for matching verse
    for(size_t i = 0; i < adapter->index_cache_size; i++) {
        VerseIndexRecord* record = &adapter->index_cache[i];
        if(record->book_id == book_index && 
           record->chapter == chapter && 
           record->verse == verse) {
            *out_offset = record->text_offset;
            *out_length = record->text_len;
            return true;
        }
    }
    
    return false;
}
