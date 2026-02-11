#include "history_manager.h"

#include <furi.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/file_stream.h>
#include <string.h>

/* History file format (simple binary):
 * - uint32_t magic (0x48495354 = "HIST")
 * - uint16_t version (1)
 * - uint16_t count
 * - HistoryEntry last_read (if valid)
 * - uint8_t last_read_valid
 * - HistoryEntry records (count * sizeof(HistoryEntry))
 */

#define HISTORY_MAGIC 0x48495354  // "HIST"
#define HISTORY_VERSION 1

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t count;
    HistoryEntry last_read;
    uint8_t last_read_valid;
} HistoryFileHeader;
#pragma pack(pop)

/* Initialize history manager */
bool history_manager_init(HistoryManager* manager) {
    if(!manager) return false;
    
    memset(manager, 0, sizeof(HistoryManager));
    manager->initialized = true;
    
    // Try to load from storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        return false;
    }
    
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    // Try to open history file
    if(file_stream_open(stream, HISTORY_STORAGE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        // Read header
        HistoryFileHeader header;
        size_t bytes_read = stream_read(stream, (uint8_t*)&header, sizeof(header));
        
        if(bytes_read == sizeof(header) && 
           header.magic == HISTORY_MAGIC && 
           header.version == HISTORY_VERSION &&
           header.count <= HISTORY_MAX_ENTRIES) {
            
            // Load last_read
            manager->last_read = header.last_read;
            manager->last_read_valid = (header.last_read_valid != 0);
            
            // Read history entries
            if(header.count > 0) {
                size_t to_read = header.count * sizeof(HistoryEntry);
                bytes_read = stream_read(stream, (uint8_t*)manager->entries, to_read);
                
                if(bytes_read == to_read) {
                    manager->count = header.count;
                    // Mark all loaded entries as valid
                    for(size_t i = 0; i < manager->count; i++) {
                        manager->entries[i].valid = true;
                    }
                }
            }
        }
        
        file_stream_close(stream);
    }
    
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    
    return true;
}

/* Cleanup history manager */
void history_manager_free(HistoryManager* manager) {
    if(!manager || !manager->initialized) return;
    
    // Save before cleanup
    history_manager_save(manager);
    
    manager->initialized = false;
}

/* Save history to storage */
bool history_manager_save(HistoryManager* manager) {
    if(!manager || !manager->initialized) return false;
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) return false;
    
    // Ensure directory exists
    storage_common_mkdir(storage, "/ext/apps_data/catholic_bible");
    
    Stream* stream = file_stream_alloc(storage);
    if(!stream) {
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    // Open for writing (create if doesn't exist, truncate if exists)
    if(!file_stream_open(stream, HISTORY_STORAGE_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    // Write header
    HistoryFileHeader header = {
        .magic = HISTORY_MAGIC,
        .version = HISTORY_VERSION,
        .count = (uint16_t)manager->count,
        .last_read = manager->last_read,
        .last_read_valid = manager->last_read_valid ? 1 : 0
    };
    
    size_t bytes_written = stream_write(stream, (uint8_t*)&header, sizeof(header));
    if(bytes_written != sizeof(header)) {
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    // Write history entries
    if(manager->count > 0) {
        size_t to_write = manager->count * sizeof(HistoryEntry);
        bytes_written = stream_write(stream, (uint8_t*)manager->entries, to_write);
        if(bytes_written != to_write) {
            file_stream_close(stream);
            stream_free(stream);
            furi_record_close(RECORD_STORAGE);
            return false;
        }
    }
    
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    
    return true;
}

/* Add entry to history */
void history_manager_add_entry(
    HistoryManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
) {
    if(!manager || !manager->initialized) return;
    if(book_index >= 73) return; // Catholic canon has 73 books
    
    // Shift existing entries right (make room at front)
    if(manager->count >= HISTORY_MAX_ENTRIES) {
        // Remove oldest (last entry)
        for(size_t i = HISTORY_MAX_ENTRIES - 1; i > 0; i--) {
            manager->entries[i] = manager->entries[i - 1];
        }
        manager->count = HISTORY_MAX_ENTRIES - 1;
    }
    
    // New entry (stack) to insert at front
    HistoryEntry new_entry = {
        .book_index = book_index,
        .chapter = chapter,
        .verse = verse,
        .valid = true,
    };
    
    // Shift all entries right to make room
    for(size_t i = manager->count; i > 0; i--) {
        manager->entries[i] = manager->entries[i - 1];
    }
    
    manager->entries[0] = new_entry;
    manager->count++;
    
    // Auto-save periodically (not on every add to avoid excessive writes)
    // Save will happen on free() or can be called explicitly
}

/* Set last-read verse */
void history_manager_set_last_read(
    HistoryManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
) {
    if(!manager || !manager->initialized) return;
    if(book_index >= 73) return;
    
    manager->last_read.book_index = book_index;
    manager->last_read.chapter = chapter;
    manager->last_read.verse = verse;
    manager->last_read.valid = true;
    manager->last_read_valid = true;
    
    // Auto-save
    history_manager_save(manager);
}

/* Get last-read verse */
bool history_manager_get_last_read(
    HistoryManager* manager,
    size_t* out_book_index,
    uint16_t* out_chapter,
    uint16_t* out_verse
) {
    if(!manager || !manager->initialized || !manager->last_read_valid) {
        return false;
    }
    
    if(out_book_index) *out_book_index = manager->last_read.book_index;
    if(out_chapter) *out_chapter = manager->last_read.chapter;
    if(out_verse) *out_verse = manager->last_read.verse;
    
    return true;
}

/* Get history entry by index */
const HistoryEntry* history_manager_get_entry(HistoryManager* manager, size_t index) {
    if(!manager || !manager->initialized) return NULL;
    if(index >= manager->count) return NULL;
    if(!manager->entries[index].valid) return NULL;
    
    return &manager->entries[index];
}

/* Get number of history entries */
size_t history_manager_count(HistoryManager* manager) {
    if(!manager || !manager->initialized) return 0;
    return manager->count;
}

/* Clear all history entries */
void history_manager_clear(HistoryManager* manager) {
    if(!manager || !manager->initialized) return;
    
    manager->count = 0;
    memset(manager->entries, 0, sizeof(manager->entries));
    
    // Auto-save
    history_manager_save(manager);
}
