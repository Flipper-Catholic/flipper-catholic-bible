#!/usr/bin/env python3
"""
Build SD card assets for Catholic Bible app (Flipper Zero).

Reads a JSON source and writes:
  - bible_text.bin  (raw UTF-8 verse texts concatenated)
  - verse_index.bin (VIDX header + records: offset, length, book_id, chapter, verse)

Usage:
  python3 tools/build_bible_assets.py [--input SOURCE.json] [--output DIR]
  Default input: assets/source/bible_source.json
  Default output: dist/apps_data/bible (create and copy to SD card /apps_data/bible/)
"""

import argparse
import json
import os
import struct
import sys
from typing import Optional

VERSE_INDEX_MAGIC = 0x56494458  # "VIDX" little-endian
VERSE_INDEX_VERSION = 1

# Packed struct layout (must match storage_adapter.h)
# VerseIndexHeader: magic(4) version(1) _pad(3) total_verses(4) = 12 bytes
# VerseIndexRecord: text_offset(4) text_len(2) book_id(1) chapter(2) verse(2) reserved(1) = 12 bytes


def write_verse_index_header(f, total_verses: int) -> None:
    # Packed struct (pragma pack 1): magic(4) + version(1) + total_verses(4) = 9 bytes
    f.write(struct.pack("<IBI", VERSE_INDEX_MAGIC, VERSE_INDEX_VERSION, total_verses))


def write_verse_index_record(f, text_offset: int, text_len: int, book_id: int, chapter: int, verse: int) -> None:
    # text_offset(4) text_len(2) book_id(1) chapter(2) verse(2) reserved(1)
    f.write(struct.pack("<I", text_offset))
    f.write(struct.pack("<H", text_len))
    f.write(struct.pack("<B", book_id))
    f.write(struct.pack("<H", chapter))
    f.write(struct.pack("<H", verse))
    f.write(struct.pack("<B", 0))  # reserved


def build(source_path: str, output_dir: str) -> None:
    with open(source_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    books = data.get("books", [])
    if not books:
        raise SystemExit("No 'books' in source JSON")

    os.makedirs(output_dir, exist_ok=True)
    path_text = os.path.join(output_dir, "bible_text.bin")
    path_index = os.path.join(output_dir, "verse_index.bin")

    total_verses = 0
    verse_list = []  # (book_id, chapter, verse, text)

    for book_id, book in enumerate(books):
        name = book.get("name", "")
        chapters = book.get("chapters", {})
        for ch_key, verses in sorted(chapters.items(), key=lambda x: int(x[0])):
            chapter = int(ch_key)
            for v_num, text in enumerate(verses, start=1):
                verse_list.append((book_id, chapter, v_num, text.strip()))
                total_verses += 1

    # Write bible_text.bin (concatenated verse texts, no separators)
    with open(path_text, "wb") as f_text:
        offset = 0
        records = []
        for book_id, chapter, verse, text in verse_list:
            raw = text.encode("utf-8")
            length = len(raw)
            records.append((offset, length, book_id, chapter, verse))
            f_text.write(raw)
            offset += length

    # Write verse_index.bin
    with open(path_index, "wb") as f_idx:
        write_verse_index_header(f_idx, total_verses)
        for rec in records:
            write_verse_index_record(f_idx, rec[0], rec[1], rec[2], rec[3], rec[4])

    print(f"Wrote {total_verses} verses to {output_dir}")
    print(f"  {path_text}")
    print(f"  {path_index}")
    print("Copy contents of the output dir to SD: /apps_data/bible/")


def find_bible_source(root: str) -> Optional[str]:
    """Try likely locations for bible_source.json (wrong-spot tolerance)."""
    candidates = [
        os.path.join(root, "assets", "source", "bible_source.json"),
        os.path.join(root, "bible_source.json"),
        os.path.join(root, "assets", "bible_source.json"),
        os.path.join(os.getcwd(), "bible_source.json"),
    ]
    for path in candidates:
        if os.path.isfile(path):
            return path
    return None


def main() -> None:
    parser = argparse.ArgumentParser(description="Build Bible SD assets for Flipper Catholic Bible app")
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    default_output = os.path.join(root, "dist", "apps_data", "bible")
    parser.add_argument("--input", "-i", default=None, help="Input JSON path (default: search then assets/source/bible_source.json)")
    parser.add_argument("--output", "-o", default=default_output, help="Output directory")
    args = parser.parse_args()
    input_path = args.input
    used_search = False
    if input_path is None:
        used_search = True
        input_path = find_bible_source(root) or os.path.join(root, "assets", "source", "bible_source.json")
    if not os.path.isfile(input_path):
        print(f"Error: input file not found: {input_path}", file=sys.stderr)
        if used_search:
            print("  Searched: assets/source/, project root, assets/, cwd. Use -i PATH to specify.", file=sys.stderr)
        sys.exit(1)
    build(input_path, args.output)


if __name__ == "__main__":
    main()
