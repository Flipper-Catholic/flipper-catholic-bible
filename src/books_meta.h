#pragma once
#include <stdint.h>

/*
 * Chapter counts for the 73-book Catholic canon, in the exact order of catholic_books[]
 * defined in src/catholic_bible.c.
 *
 * Notes:
 * - Daniel is counted as 14 chapters (incl. Susanna; Bel & the Dragon).
 * - Baruch is counted as 6 chapters (incl. Letter of Jeremiah as ch. 6).
 * - Esther is 16 in common Catholic enumerations that include additions.
 */
static const uint16_t catholic_book_chapter_counts[] = {
    50, 40, 27, 36, 34,  // Pentateuch
    24, 21, 4,           // History
    31, 24, 22, 25,
    29, 36,
    10, 13, 14, 16, 16,
    16, 15,
    42, 150, 31, 12,
    8, 19, 51,
    66, 52, 5, 6,
    48, 14,
    14, 3, 9, 1, 4, 7,
    3, 3, 3, 2, 14, 4,
    28, 16, 24, 21, 28,
    16, 16, 13, 6,
    6, 4, 4,
    5, 3,
    6, 4, 3, 1,
    13, 5, 5, 3,
    5, 1, 1, 1,
    22,
};
