#include "bookmark_manager.h"

#include <furi.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/file_stream.h>
#include <string.h>
#include <stdio.h>

/* Bookmark file format (simple binary):
 * - uint32_t magic (0x424D524B = "BMRK")
 * - uint16_t version (1)
 * - uint16_t count
 * - Bookmark records (count * sizeof(Bookmark))
 */

#define BOOKMARK_MAGIC 0x424D524B  // "BMRK"
#define BOOKMARK_VERSION 1

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t count;
} BookmarkFileHeader;
#pragma pack(pop)

/* Initialize bookmark manager */
bool bookmark_manager_init(BookmarkManager* manager) {
    if(!manager) return false;
    
    memset(manager, 0, sizeof(BookmarkManager));
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
    
    // Try to open bookmark file
    if(file_stream_open(stream, BOOKMARK_STORAGE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        // Read header
        BookmarkFileHeader header;
        size_t bytes_read = stream_read(stream, (uint8_t*)&header, sizeof(header));
        
        if(bytes_read == sizeof(header) && 
           header.magic == BOOKMARK_MAGIC && 
           header.version == BOOKMARK_VERSION &&
           header.count <= BOOKMARK_MAX_COUNT) {
            
            // Read bookmarks
            size_t to_read = header.count * sizeof(Bookmark);
            bytes_read = stream_read(stream, (uint8_t*)manager->bookmarks, to_read);
            
            if(bytes_read == to_read) {
                manager->count = header.count;
                // Mark all loaded bookmarks as valid
                for(size_t i = 0; i < manager->count; i++) {
                    manager->bookmarks[i].valid = true;
                }
            }
        }
        
        file_stream_close(stream);
    }
    
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);
    
    return true;
}

/* Cleanup bookmark manager */
void bookmark_manager_free(BookmarkManager* manager) {
    if(!manager || !manager->initialized) return;
    
    // Save before cleanup
    bookmark_manager_save(manager);
    
    manager->initialized = false;
}

/* Save bookmarks to storage */
bool bookmark_manager_save(BookmarkManager* manager) {
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
    if(!file_stream_open(stream, BOOKMARK_STORAGE_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    // Write header
    BookmarkFileHeader header = {
        .magic = BOOKMARK_MAGIC,
        .version = BOOKMARK_VERSION,
        .count = (uint16_t)manager->count
    };
    
    size_t bytes_written = stream_write(stream, (uint8_t*)&header, sizeof(header));
    if(bytes_written != sizeof(header)) {
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    
    // Write bookmarks
    if(manager->count > 0) {
        size_t to_write = manager->count * sizeof(Bookmark);
        bytes_written = stream_write(stream, (uint8_t*)manager->bookmarks, to_write);
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

/* Add a new bookmark */
bool bookmark_manager_add(
    BookmarkManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse,
    const char* name
) {
    if(!manager || !manager->initialized) return false;
    if(manager->count >= BOOKMARK_MAX_COUNT) return false;
    if(book_index >= 73) return false; // Catholic canon has 73 books
    
    // Check if already bookmarked
    if(bookmark_manager_is_bookmarked(manager, book_index, chapter, verse)) {
        return false; // Already exists
    }
    
    // Find empty slot or add to end
    size_t index = manager->count;
    Bookmark* bookmark = &manager->bookmarks[index];
    
    bookmark->book_index = book_index;
    bookmark->chapter = chapter;
    bookmark->verse = verse;
    bookmark->valid = true;
    
    // Copy name (truncate if too long)
    if(name) {
        strncpy(bookmark->name, name, BOOKMARK_NAME_MAX_LEN - 1);
        bookmark->name[BOOKMARK_NAME_MAX_LEN - 1] = '\0';
    } else {
        bookmark->name[0] = '\0';
    }
    
    manager->count++;
    
    // Auto-save
    bookmark_manager_save(manager);
    
    return true;
}

/* Delete a bookmark by index */
bool bookmark_manager_delete(BookmarkManager* manager, size_t index) {
    if(!manager || !manager->initialized) return false;
    if(index >= manager->count) return false;
    
    // Shift remaining bookmarks left
    for(size_t i = index; i < manager->count - 1; i++) {
        manager->bookmarks[i] = manager->bookmarks[i + 1];
    }
    
    manager->count--;
    manager->bookmarks[manager->count].valid = false;
    
    // Auto-save
    bookmark_manager_save(manager);
    
    return true;
}

/* Get bookmark by index */
const Bookmark* bookmark_manager_get(BookmarkManager* manager, size_t index) {
    if(!manager || !manager->initialized) return NULL;
    if(index >= manager->count) return NULL;
    if(!manager->bookmarks[index].valid) return NULL;
    
    return &manager->bookmarks[index];
}

/* Get number of bookmarks */
size_t bookmark_manager_count(BookmarkManager* manager) {
    if(!manager || !manager->initialized) return 0;
    return manager->count;
}

/* Find bookmark by verse location */
int bookmark_manager_find(
    BookmarkManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
) {
    if(!manager || !manager->initialized) return -1;
    
    for(size_t i = 0; i < manager->count; i++) {
        Bookmark* bm = &manager->bookmarks[i];
        if(bm->valid &&
           bm->book_index == book_index &&
           bm->chapter == chapter &&
           bm->verse == verse) {
            return (int)i;
        }
    }
    
    return -1;
}

/* Check if a verse is bookmarked */
bool bookmark_manager_is_bookmarked(
    BookmarkManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
) {
    return bookmark_manager_find(manager, book_index, chapter, verse) >= 0;
}
