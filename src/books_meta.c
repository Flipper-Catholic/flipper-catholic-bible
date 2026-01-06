#include "books_meta.h"

const char* const catholic_bible_book_names[CATHOLIC_BIBLE_BOOKS_COUNT] = {
    // Old Testament (46)
    "Genesis","Exodus","Leviticus","Numbers","Deuteronomy",
    "Joshua","Judges","Ruth",
    "1 Samuel","2 Samuel","1 Kings","2 Kings",
    "1 Chronicles","2 Chronicles","Ezra","Nehemiah",
    "Tobit","Judith","Esther",
    "1 Maccabees","2 Maccabees",
    "Job","Psalms","Proverbs","Ecclesiastes","Song of Songs",
    "Wisdom","Sirach",
    "Isaiah","Jeremiah","Lamentations","Baruch","Ezekiel","Daniel",
    "Hosea","Joel","Amos","Obadiah","Jonah","Micah","Nahum","Habakkuk",
    "Zephaniah","Haggai","Zechariah","Malachi",

    // New Testament (27)
    "Matthew","Mark","Luke","John","Acts",
    "Romans","1 Corinthians","2 Corinthians","Galatians","Ephesians","Philippians","Colossians",
    "1 Thessalonians","2 Thessalonians",
    "1 Timothy","2 Timothy","Titus","Philemon",
    "Hebrews","James","1 Peter","2 Peter","1 John","2 John","3 John","Jude","Revelation"
};

const uint16_t catholic_bible_book_chapter_counts[CATHOLIC_BIBLE_BOOKS_COUNT] = {
    // Old Testament (46)
    50,40,27,36,34,        // Pentateuch
    24,21,4,               // Joshua, Judges, Ruth
    31,24,22,25,           // 1-2 Sam, 1-2 Kings
    29,36,10,13,           // 1-2 Chr, Ezra, Neh
    14,16,16,              // Tobit, Judith, Esther (Catholic w/ additions often 16)
    16,15,                 // 1-2 Maccabees
    42,150,31,12,8,        // Job, Psalms, Proverbs, Ecclesiastes, Song
    19,51,                 // Wisdom, Sirach
    66,52,5,6,48,14,       // Isaiah, Jeremiah, Lam, Baruch(6), Ezekiel, Daniel(14)
    14,3,9,1,4,7,3,3,      // Hosea..Habakkuk
    3,2,14,4,              // Zeph, Haggai, Zech, Malachi

    // New Testament (27)
    28,16,24,21,28,        // Gospels+Acts
    16,16,13,6,6,4,4,      // Paul (part 1)
    5,3,                   // Thess
    6,4,3,1,               // Pastoral + Philemon
    13,5,5,3,5,1,1,1,22    // Hebrews..Revelation
};
