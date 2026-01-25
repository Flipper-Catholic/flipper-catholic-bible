#pragma once
#include <stdint.h>

#define CATHOLIC_BIBLE_BOOKS_COUNT 73
#define MAX_CHAPTERS_PER_BOOK 150  // Psalms has 150 chapters

extern const char* const catholic_bible_book_names[CATHOLIC_BIBLE_BOOKS_COUNT];
extern const uint16_t catholic_bible_book_chapter_counts[CATHOLIC_BIBLE_BOOKS_COUNT];

// Verse counts: [book_index][chapter_1based - 1] = verse_count
// Note: chapter is 1-based in usage, but array is 0-indexed
extern const uint16_t catholic_bible_verse_counts[CATHOLIC_BIBLE_BOOKS_COUNT][MAX_CHAPTERS_PER_BOOK];
