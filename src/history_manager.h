#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* History Manager for Catholic Bible App
 * Phase 4.2: History Manager
 * Tracks recent reading history and last-read verse
 */

#define HISTORY_MAX_ENTRIES 20
#define HISTORY_STORAGE_PATH "/ext/apps_data/catholic_bible/history.dat"

/* History entry structure */
typedef struct {
    size_t book_index;      // 0-72 (Catholic canon)
    uint16_t chapter;       // 1-based chapter
    uint16_t verse;         // 1-based verse
    bool valid;             // True if entry is valid/used
} HistoryEntry;

/* History Manager state */
typedef struct {
    HistoryEntry entries[HISTORY_MAX_ENTRIES];
    size_t count;           // Number of valid entries
    HistoryEntry last_read; // Last-read verse (separate from history list)
    bool last_read_valid;   // True if last_read is valid
    bool initialized;
} HistoryManager;

/* Initialize history manager and load from storage */
bool history_manager_init(HistoryManager* manager);

/* Cleanup history manager */
void history_manager_free(HistoryManager* manager);

/* Save history to storage */
bool history_manager_save(HistoryManager* manager);

/* Add entry to history (most recent first)
 * Automatically removes oldest if at max capacity
 */
void history_manager_add_entry(
    HistoryManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
);

/* Set last-read verse */
void history_manager_set_last_read(
    HistoryManager* manager,
    size_t book_index,
    uint16_t chapter,
    uint16_t verse
);

/* Get last-read verse
 * Returns true if last_read is valid
 */
bool history_manager_get_last_read(
    HistoryManager* manager,
    size_t* out_book_index,
    uint16_t* out_chapter,
    uint16_t* out_verse
);

/* Get history entry by index (0 = most recent) */
const HistoryEntry* history_manager_get_entry(HistoryManager* manager, size_t index);

/* Get number of history entries */
size_t history_manager_count(HistoryManager* manager);

/* Clear all history entries */
void history_manager_clear(HistoryManager* manager);
