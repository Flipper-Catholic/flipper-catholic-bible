#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Bookmark Manager for Catholic Bible App
 * Phase 4.1: Bookmark Manager
 * Stores bookmarks in Flipper app storage (not SD card)
 */

#define BOOKMARK_MAX_COUNT 50
#define BOOKMARK_NAME_MAX_LEN 32
#define BOOKMARK_STORAGE_PATH "/ext/apps_data/catholic_bible/bookmarks.dat"

/* Bookmark structure */
typedef struct {
    size_t book_index;      // 0-72 (Catholic canon)
    uint16_t chapter;       // 1-based chapter
    uint16_t verse;         // 1-based verse
    char name[BOOKMARK_NAME_MAX_LEN]; // User-friendly name (e.g., "Genesis 1:1")
    bool valid;             // True if bookmark is valid/used
} Bookmark;

/* Bookmark Manager state */
typedef struct {
    Bookmark bookmarks[BOOKMARK_MAX_COUNT];
    size_t count;           // Number of valid bookmarks
    bool initialized;
} BookmarkManager;

/* Initialize bookmark manager and load from storage */
bool bookmark_manager_init(BookmarkManager* manager);

/* Cleanup bookmark manager */
void bookmark_manager_free(BookmarkManager* manager);

/* Save bookmarks to storage */
bool bookmark_manager_save(BookmarkManager* manager);

/* Add a new bookmark
 * Returns true on success, false if max count reached or error
 */
bool bookmark_manager_add(
    BookmarkManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse,
    const char* name
);

/* Delete a bookmark by index
 * Returns true on success
 */
bool bookmark_manager_delete(BookmarkManager* manager, size_t index);

/* Get bookmark by index
 * Returns NULL if index is invalid
 */
const Bookmark* bookmark_manager_get(BookmarkManager* manager, size_t index);

/* Get number of bookmarks */
size_t bookmark_manager_count(BookmarkManager* manager);

/* Find bookmark by verse location
 * Returns index if found, -1 if not found
 */
int bookmark_manager_find(
    BookmarkManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
);

/* Check if a verse is bookmarked */
bool bookmark_manager_is_bookmarked(
    BookmarkManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
);
