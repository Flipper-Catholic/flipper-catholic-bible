#!/usr/bin/env python3
"""
Build search index for Catholic Bible app (Phase 3).

Reads bible_source.json (or --from-verse-index with bible_text.bin), tokenizes verse text,
builds sharded inverted index (token -> verse_ids). Writes:
  - search_shard_map.bin   (2-char prefix -> shard index)
  - search_shards/shard_*.bin (token dictionary + posting lists)

Run after build_bible_assets.py, or with same JSON input.
"""

import argparse
import json
import os
import re
import struct
import sys
from collections import defaultdict
from typing import List, Tuple

SEARCH_MAGIC = 0x53494458  # "SIDX"
SEARCH_VERSION = 1
PREFIX_CHARS = 26  # a-z
SHARD_MAP_SIZE = PREFIX_CHARS * PREFIX_CHARS  # 676
MIN_TOKEN_LEN = 2
MAX_TOKEN_LEN = 32


def tokenize(text: str) -> List[str]:
    """Lowercase, split on non-alpha, keep tokens >= MIN_TOKEN_LEN."""
    text = re.sub(r"\*+", " ", text)  # footnote markers
    words = re.findall(r"[a-zA-Z]+", text)
    out = []
    for w in words:
        w = w.lower()
        if MIN_TOKEN_LEN <= len(w) <= MAX_TOKEN_LEN:
            out.append(w)
    return out


def prefix_to_index(prefix: str) -> int:
    """Map 2-char prefix to 0..675. Non-alpha -> 0."""
    if len(prefix) < 2:
        return 0
    a, b = prefix[0], prefix[1]
    if not a.isalpha() or not b.isalpha():
        return 0
    return (ord(a.lower()) - ord("a")) * PREFIX_CHARS + (ord(b.lower()) - ord("a"))


def build_from_json(source_path: str) -> List[Tuple[int, int, int, str]]:
    """Return verse_list: (book_id, chapter, verse_1based, text). verse_id = index in list."""
    with open(source_path, "r", encoding="utf-8") as f:
        data = json.load(f)
    verse_list = []
    for book_id, book in enumerate(data.get("books", [])):
        for ch_key, verses in sorted(book.get("chapters", {}).items(), key=lambda x: int(x[0])):
            chapter = int(ch_key)
            for v_num, text in enumerate(verses, start=1):
                verse_list.append((book_id, chapter, v_num, text.strip()))
    return verse_list


def build_index(verse_list: List[Tuple[int, int, int, str]]) -> dict:
    """Inverted index: token -> sorted list of verse_ids (0-based index)."""
    inv: dict = defaultdict(list)
    for verse_id, (_, _, _, text) in enumerate(verse_list):
        for token in tokenize(text):
            inv[token].append(verse_id)
    for k in inv:
        inv[k] = sorted(set(inv[k]))
    return inv


def shard_index(inv: dict) -> dict:
    """Group by 2-char prefix. prefix -> sorted list of (token, verse_ids)."""
    shards = defaultdict(list)
    for token, verse_ids in inv.items():
        pref = token[:2].lower() if len(token) >= 2 else "aa"
        if not pref[0].isalpha():
            pref = "aa"
        if len(pref) == 1:
            pref = pref + "a"
        shards[pref].append((token, verse_ids))
    for pref in shards:
        shards[pref].sort(key=lambda x: x[0])
    return shards


def write_shard_map(shards: dict, output_dir: str) -> None:
    """Write search_shard_map.bin. Prefix index i -> shard_id (0..n). Unused = 0xFFFF."""
    path = os.path.join(output_dir, "search_shard_map.bin")
    shard_id = 0
    prefix_to_shard = [0xFFFF] * SHARD_MAP_SIZE
    for i in range(SHARD_MAP_SIZE):
        ai, bi = i // PREFIX_CHARS, i % PREFIX_CHARS
        pref = chr(ord("a") + ai) + chr(ord("a") + bi)
        if pref in shards:
            prefix_to_shard[i] = shard_id
            shard_id += 1
    with open(path, "wb") as f:
        f.write(struct.pack("<I", SEARCH_MAGIC))
        f.write(struct.pack("<H", SEARCH_VERSION))
        f.write(struct.pack("<H", SHARD_MAP_SIZE))
        for v in prefix_to_shard:
            f.write(struct.pack("<H", v))


def write_shards(shards: dict, output_dir: str) -> None:
    """Write search_shards/shard_*.bin in prefix order (aa, ab, ...)."""
    shards_dir = os.path.join(output_dir, "search_shards")
    os.makedirs(shards_dir, exist_ok=True)
    shard_id = 0
    for i in range(SHARD_MAP_SIZE):
        ai, bi = i // PREFIX_CHARS, i % PREFIX_CHARS
        pref = chr(ord("a") + ai) + chr(ord("a") + bi)
        if pref not in shards:
            continue
        entries = shards[pref]
        path = os.path.join(shards_dir, f"shard_{shard_id:03d}.bin")
        with open(path, "wb") as f:
            f.write(struct.pack("<I", SEARCH_MAGIC))
            f.write(struct.pack("<H", SEARCH_VERSION))
            f.write(struct.pack("<I", len(entries)))
            for token, verse_ids in entries:
                token_b = token.encode("utf-8")[:MAX_TOKEN_LEN]
                f.write(struct.pack("<B", len(token_b)))
                f.write(token_b)
                f.write(struct.pack("<H", len(verse_ids)))
                for vid in verse_ids:
                    f.write(struct.pack("<I", vid))
        shard_id += 1


def main() -> None:
    parser = argparse.ArgumentParser(description="Build search index for Catholic Bible app")
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    parser.add_argument("--input", "-i", default=None, help="bible_source.json path")
    parser.add_argument("--output", "-o", default=os.path.join(root, "files"), help="Output directory")
    args = parser.parse_args()
    input_path = args.input or os.path.join(root, "assets", "source", "bible_source.json")
    if not os.path.isfile(input_path):
        print(f"Error: not found {input_path}", file=sys.stderr)
        sys.exit(1)
    verse_list = build_from_json(input_path)
    print(f"Indexing {len(verse_list)} verses...")
    inv = build_index(verse_list)
    shards = shard_index(inv)
    os.makedirs(args.output, exist_ok=True)
    write_shard_map(shards, args.output)
    write_shards(shards, args.output)
    print(f"Wrote search index: {len(shards)} shards in {args.output}")


if __name__ == "__main__":
    main()
