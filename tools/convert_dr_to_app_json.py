#!/usr/bin/env python3
"""
Convert Douay-Rheims JSON (e.g. xxruyle/Bible-DouayRheims format) to app bible_source.json.

Input format (xxruyle style): top-level keys = book names; each value is:
  { "1": { "1": "verse text", "2": "...", ... }, "2": { ... }, ... }
  (chapter number -> verse number -> text)

Output format (app): { "books": [ { "name": "...", "chapters": { "1": ["v1", "v2", ...], ... } }, ... ] }
  Book order = Catholic canon (73 books), matching src/books_meta.c.

Usage:
  python3 tools/convert_dr_to_app_json.py --input EntireBible-DR.json --output assets/source/bible_source.json
"""

import argparse
import json
import os
import re
import sys

# Catholic canon order (must match src/books_meta.c)
CANONICAL_BOOK_NAMES = [
    "Genesis", "Exodus", "Leviticus", "Numbers", "Deuteronomy",
    "Joshua", "Judges", "Ruth",
    "1 Samuel", "2 Samuel", "1 Kings", "2 Kings",
    "1 Chronicles", "2 Chronicles", "Ezra", "Nehemiah",
    "Tobit", "Judith", "Esther",
    "1 Maccabees", "2 Maccabees",
    "Job", "Psalms", "Proverbs", "Ecclesiastes", "Song of Songs",
    "Wisdom", "Sirach",
    "Isaiah", "Jeremiah", "Lamentations", "Baruch", "Ezekiel", "Daniel",
    "Hosea", "Joel", "Amos", "Obadiah", "Jonah", "Micah", "Nahum", "Habakkuk",
    "Zephaniah", "Haggai", "Zechariah", "Malachi",
    "Matthew", "Mark", "Luke", "John", "Acts",
    "Romans", "1 Corinthians", "2 Corinthians", "Galatians", "Ephesians", "Philippians", "Colossians",
    "1 Thessalonians", "2 Thessalonians",
    "1 Timothy", "2 Timothy", "Titus", "Philemon",
    "Hebrews", "James", "1 Peter", "2 Peter", "1 John", "2 John", "3 John", "Jude", "Revelation",
]

# Map source/alternate book names (lowercase) to canonical (matches books_meta.c)
ALIAS_TO_CANONICAL = {
    "song of solomon": "Song of Songs",
    "song of songs": "Song of Songs",
    "canticles": "Song of Songs",
    "ecclesiasticus": "Sirach",
    "sirach": "Sirach",
    "wisdom of solomon": "Wisdom",
    "wisdom": "Wisdom",
    "1 samuel": "1 Samuel",
    "2 samuel": "2 Samuel",
    "1 kings": "1 Samuel",   # DR/Latin: 1 Kings = 1 Samuel
    "2 kings": "2 Samuel",   # 2 Kings = 2 Samuel
    "3 kings": "1 Kings",
    "4 kings": "2 Kings",
    "1 chronicles": "1 Chronicles",
    "2 chronicles": "2 Chronicles",
    "1 paralipomenon": "1 Chronicles",
    "2 paralipomenon": "2 Chronicles",
    "1 esdras": "Ezra",
    "2 esdras": "Nehemiah",
    "tobias": "Tobit",
    "tobit": "Tobit",
    "1 machabees": "1 Maccabees",
    "2 machabees": "2 Maccabees",
    "1 maccabees": "1 Maccabees",
    "2 maccabees": "2 Maccabees",
    "josue": "Joshua",
    "joshua": "Joshua",
    "isaias": "Isaiah",
    "isaiah": "Isaiah",
    "jeremias": "Jeremiah",
    "jeremiah": "Jeremiah",
    "ezechiel": "Ezekiel",
    "ezekiel": "Ezekiel",
    "osee": "Hosea",
    "hosea": "Hosea",
    "abdias": "Obadiah",
    "obadiah": "Obadiah",
    "jonas": "Jonah",
    "jonah": "Jonah",
    "micheas": "Micah",
    "micah": "Micah",
    "habacuc": "Habakkuk",
    "habakkuk": "Habakkuk",
    "sophonias": "Zephaniah",
    "zephaniah": "Zephaniah",
    "aggeus": "Haggai",
    "haggai": "Haggai",
    "zacharias": "Zechariah",
    "zechariah": "Zechariah",
    "malachias": "Malachi",
    "malachi": "Malachi",
    "1 corinthians": "1 Corinthians",
    "2 corinthians": "2 Corinthians",
    "1 thessalonians": "1 Thessalonians",
    "2 thessalonians": "2 Thessalonians",
    "1 timothy": "1 Timothy",
    "2 timothy": "2 Timothy",
    "1 peter": "1 Peter",
    "2 peter": "2 Peter",
    "1 john": "1 John",
    "2 john": "2 John",
    "3 john": "3 John",
    "revelation": "Revelation",
    "apocalypse": "Revelation",
}


def normalize_book_name(name: str) -> str:
    if not name:
        return ""
    key = name.strip().lower()
    return ALIAS_TO_CANONICAL.get(key, name.strip())


def book_name_matches(canonical: str, source_key: str) -> bool:
    n = normalize_book_name(source_key)
    if n and n.lower() == canonical.lower():
        return True
    if canonical.lower() == n.lower():
        return True
    return False


def chapter_verse_dict_to_list(chapter_dict: dict) -> list:
    """Convert { "1": "text", "2": "text", ... } to ["text", "text", ...] in order."""
    if not isinstance(chapter_dict, dict):
        return []
    keys = sorted(chapter_dict.keys(), key=lambda x: int(x) if str(x).isdigit() else 0)
    return [str(chapter_dict[k]).strip() for k in keys if str(chapter_dict.get(k, "")).strip()]


def convert_book(source_book: dict) -> dict:
    """source_book: { "1": { "1": "..", "2": ".." }, "2": { ... } } -> chapters: { "1": [...], "2": [...] }."""
    chapters = {}
    ch_keys = sorted(source_book.keys(), key=lambda x: int(x) if str(x).isdigit() else 0)
    for ch in ch_keys:
        cv = source_book[ch]
        if isinstance(cv, dict):
            chapters[str(ch)] = chapter_verse_dict_to_list(cv)
        elif isinstance(cv, list):
            chapters[str(ch)] = [str(t).strip() for t in cv]
        else:
            chapters[str(ch)] = []
    return chapters


def load_source(path: str) -> dict:
    with open(path, "r", encoding="utf-8") as f:
        data = json.load(f)
    # If it's already { "books": [ ... ] } in app format, return as-is (no conversion)
    if "books" in data and isinstance(data["books"], list):
        return data
    # Otherwise assume top-level keys are book names (xxruyle style)
    return data


def convert(source_path: str, output_path: str) -> None:
    raw = load_source(source_path)

    if "books" in raw and isinstance(raw["books"], list):
        # Already in app format; optionally reorder by canon
        books_in = {b["name"]: b for b in raw["books"] if b.get("name")}
        books_out = []
        for canon_name in CANONICAL_BOOK_NAMES:
            if canon_name in books_in:
                books_out.append(books_in[canon_name])
            else:
                match = next((b for b in raw["books"] if book_name_matches(canon_name, b.get("name", ""))), None)
                if match:
                    match["name"] = canon_name
                    books_out.append(match)
        result = {"books": books_out}
    else:
        # xxruyle style: top-level = book name -> chapters
        books_out = []
        for canon_name in CANONICAL_BOOK_NAMES:
            source_key = None
            for k in raw.keys():
                if book_name_matches(canon_name, k):
                    source_key = k
                    break
            if source_key is None:
                print(f"Warning: no source for book '{canon_name}'", file=sys.stderr)
                books_out.append({"name": canon_name, "chapters": {}})
                continue
            src_book = raw[source_key]
            if not isinstance(src_book, dict):
                books_out.append({"name": canon_name, "chapters": {}})
                continue
            chapters = convert_book(src_book)
            books_out.append({"name": canon_name, "chapters": chapters})

        result = {"books": books_out}

    os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(result, f, ensure_ascii=False, indent=2)

    total_verses = sum(
        sum(len(ch) for ch in b.get("chapters", {}).values())
        for b in result["books"]
    )
    print(f"Wrote {output_path} ({len(result['books'])} books, {total_verses} verses)")


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert DR JSON to app bible_source.json")
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    default_out = os.path.join(root, "assets", "source", "bible_source.json")
    parser.add_argument("--input", "-i", required=True, help="Input JSON (e.g. EntireBible-DR.json)")
    parser.add_argument("--output", "-o", default=default_out, help="Output app-format JSON")
    args = parser.parse_args()
    if not os.path.isfile(args.input):
        print(f"Error: input file not found: {args.input}", file=sys.stderr)
        sys.exit(1)
    convert(args.input, args.output)


if __name__ == "__main__":
    main()
