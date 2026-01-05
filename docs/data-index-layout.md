# Data & Index File Layout Specification
Catholic Bible App for Flipper Zero

This document defines the on-disk binary asset formats stored on the microSD card.
All formats are designed for deterministic access, low RAM usage, and offline operation.

---

## Asset Directory
All Bible assets must live at:

/apps_data/bible/

Required files:
- bible_text.bin
- verse_index.bin
- canon_table.bin
- shard_map.bin
- shards/shard_*.bin
- metadata.json

---

## General Rules
- Endianness: Little-endian
- Text encoding: UTF-8
- All binary files include a magic header and version
- Assets are read-only at runtime

---

## Verse Identity Model
Each verse has:
- A sequential numeric VerseID (uint32)
- A canonical tuple: BookID, Chapter, Verse

VerseIDs are assigned in canonical order from Genesis to Revelation.

---

## bible_text.bin
Stores all verse texts sequentially.

- No header
- Raw UTF-8 verse strings
- Offsets and lengths provided by verse_index.bin

---

## verse_index.bin
Maps VerseID to text and canonical location.

Header:
- Magic: VIDX
- Version: 1
- Total verses

Each record:
- text_offset (uint32)
- text_len (uint16)
- book_id (uint8)
- chapter (uint16, 1-based)
- verse (uint16, 1-based)
- reserved

---

## canon_table.bin
Defines canonical ordering and navigation structure.

Stores:
- BookID → Book name
- Chapter counts per book
- First VerseID per chapter

Used for:
- Pickers
- Navigation validation

---

## Search Index Overview
Search uses a sharded inverted index:
- token → list of VerseIDs
- shards selected via prefix lookup

Partial-word matching is implemented via prefix matching.

---

## shard_map.bin
Maps normalized token prefixes to shard IDs.

- Prefix length: 2 characters
- Every possible prefix maps to exactly one shard
- Empty shards are permitted

---

## shards/shard_XX.bin
Each shard contains:
- Token dictionary (sorted)
- Posting lists (VerseIDs)
- Offsets into posting arrays

No compression in v1.

---

## metadata.json
Human-readable manifest containing:
- schema_version
- translation
- canon
- asset_version
- total_books
- total_verses
- file sizes
- SHA-256 hashes

Used for:
- Compatibility checks
- Guided recovery
- Diagnostics

---

## Failure Handling
- Missing or corrupt assets trigger guided recovery mode
- If text assets load but search assets are missing, reading remains available

---

## Versioning Rules
- schema_version controls binary compatibility
- asset_version may change independently
- App supports schema_version == 1 only
