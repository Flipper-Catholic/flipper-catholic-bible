# Asset Build Pipeline Specification
Catholic Bible App for Flipper Zero

This document defines how Douay-Rheims source text is converted into deterministic SD card assets.

Goals:
- Deterministic outputs
- Stable BookID and VerseID mapping
- Correct, auditable indexing
- Reproducible builds

---

## Inputs
- Canonical Douay-Rheims source (structured: USFM/OSIS/JSON preferred)
- Canon order config: assets/config/canon_order_catholic_full.json
- Build config: assets/config/build_config.json

---

## Outputs
Emit to:
/apps_data/bible/

- bible_text.bin
- verse_index.bin
- canon_table.bin
- shard_map.bin
- shards/shard_*.bin
- metadata.json

All outputs must conform to docs/data-index-layout.md

---

## Pipeline Stages

### Stage 0: Validate Source
- All required books present
- Chapters/verses well-formed
- No duplicates
- Report missing verses/books

### Stage 1: Canonicalize Structure
Normalize into a single internal model:
- book_name
- chapter number
- verse number
- verse text (UTF-8)

Display text preserved exactly (punctuation/case).

### Stage 2: Assign IDs
BookID:
- assigned by canon order config position

VerseID:
- sequential in canonical order across full canon

### Stage 3: Emit canon_table.bin
- book names
- chapter counts
- first VerseID per chapter

### Stage 4: Emit bible_text.bin and verse_index.bin
bible_text.bin:
- concatenate verse texts in VerseID order
verse_index.bin:
- record offsets/lengths and canonical tuple

### Stage 5: Normalize and Tokenize (Indexing Only)
Aggressive normalization:
- lowercase
- strip punctuation
- diacritics removed
- collapse whitespace

Tokenize on whitespace.

### Stage 6: Build Inverted Index
Construct token → VerseID postings:
- deduplicate VerseIDs per token
- sort postings ascending

### Stage 7: Shard Index
Shard key:
- first two normalized characters
- 1-char tokens use '_' as second
- non [a-z0-9] mapped to '#'

### Stage 8: Emit shard_map.bin
- complete prefix → shard mapping

### Stage 9: Emit shard files
Each shard:
- sorted token dictionary
- token string pool
- postings lists (uint32 VerseID arrays)
No compression in v1.

### Stage 10: Emit metadata.json
Include:
- schema_version
- translation/canon
- asset_version
- counts
- file sizes
- SHA-256 hashes

Provide a deterministic mode that fixes or omits timestamps.

---

## Tooling Contract
Provide a reference CLI tool:
- tools/build_assets.py

Example:
python3 tools/build_assets.py --input <source> --out dist/apps_data/bible --deterministic

Exit codes:
- 0 success
- 1 validation error
- 2 build error
- 3 integrity error

---

## Testing Hooks
- golden dataset for CI
- random verse spot-checks
- token → verse containment verification post-normalization
