# Full Bible Acquisition Plan
Catholic Bible App for Flipper Zero

This document outlines how to obtain and deploy **all verses** of the Douay-Rheims Bible (Catholic canon, 73 books) for use with the app.

---

## Goals

1. **Source**: Obtain full Douay-Rheims text in a machine-readable format (public domain).
2. **Format**: Convert to the app’s JSON format used by `tools/build_bible_assets.py`.
3. **Build**: Run the build script to produce `bible_text.bin` and `verse_index.bin`.
4. **Deploy**: Copy assets to Flipper SD at `/apps_data/bible/`.
5. **Verse counts** (optional): Add verse-count data for all 73 books in `books_meta.c` so the verse list is accurate when SD is not present.

---

## Required Data

- **Book order**: Must match `src/books_meta.c` (Catholic canon: 46 OT + 27 NT). See `catholic_bible_book_names[]`.
- **Per book**: Correct chapter counts (already in `books_meta.c`).
- **Per chapter**: Array of verse strings in order (verse 1, 2, 3, …).

---

## Source Options (Public Domain)

### 1. **xxruyle/Bible-DouayRheims** (recommended)

- **URL**: https://github.com/xxruyle/Bible-DouayRheims  
- **File**: `EntireBible-DR.json` (full 73 books, Douay-Rheims).  
- **Format**: Top-level keys = book names; each book has `"1"`, `"2"`, … (chapters); each chapter has `"1"`, `"2"`, … (verse number) → verse text.  
- **License**: MIT.

**Use**: Run `tools/convert_dr_to_app_json.py` (see below) to convert this file into the app’s `bible_source.json` format.

### 2. **Project Gutenberg**

- **URL**: https://www.gutenberg.org/ebooks/8300 (Douay-Rheims, Challoner revision).  
- **Format**: Plain text; not per-verse. Would require a separate parser to split into book/chapter/verse.

### 3. **Other JSON / USFM / OSIS**

- If you have another JSON (e.g. `books.json` + `verses.json`) or USFM/OSIS, you can either:
  - Extend `convert_dr_to_app_json.py` to accept that format, or  
  - Convert it externally to the app’s format (see “App JSON format” below).

---

## App JSON Format

Input to `build_bible_assets.py` is a single JSON file with this structure:

```json
{
  "books": [
    { "name": "Genesis", "chapters": { "1": ["verse1", "verse2", ...], "2": [...] } },
    { "name": "Exodus",  "chapters": { "1": [...], ... } },
    ...
  ]
}
```

- `books` order must match the Catholic canon in `books_meta.c`.  
- `chapters` keys are chapter numbers as strings (`"1"`, `"2"`, …).  
- Each chapter value is an array of verse strings in order (index 0 = verse 1).

---

## Steps to Get All Verses

### Step 1: Obtain source JSON

- **Option A**: Clone or download `EntireBible-DR.json` from  
  https://github.com/xxruyle/Bible-DouayRheims (e.g. `main` branch).

- **Option B**: Place any JSON that matches the “xxruyle-style” format (book name → chapters → verse number → text) in the repo or pass its path to the converter.

### Step 2: Convert to app format

From the project root:

```bash
python3 tools/convert_dr_to_app_json.py \
  --input /path/to/EntireBible-DR.json \
  --output assets/source/bible_source.json
```

If the source uses different book names, the script maps them to the canonical names in `books_meta.c` (see script for mapping).

### Step 3: Build SD assets

```bash
python3 tools/build_bible_assets.py \
  --input assets/source/bible_source.json \
  --output dist/apps_data/bible
```

### Step 4: Deploy to Flipper

- Copy everything in `dist/apps_data/bible/` to the Flipper microSD card at `/apps_data/bible/` (create the folder if needed).  
- Required files on SD: `bible_text.bin`, `verse_index.bin`.

### Step 5 (optional): Verse counts in app

- For accurate verse lists when SD is not present (or for validation), add verse counts for all 73 books to `catholic_bible_verse_counts` in `src/books_meta.c`.  
- Counts can be derived from the same source JSON (e.g. length of each chapter array) or from a reference table.

---

## Size and Performance

- **Full Bible**: ~35,000+ verses; `bible_text.bin` is typically a few MB; `verse_index.bin` is smaller.  
- The build script and storage adapter support large files; the index is loaded into RAM on first use (consider device memory if you add more content later).  
- SD card should have enough free space for `bible_text.bin` + `verse_index.bin`.

---

## Summary

| Step | Action |
|------|--------|
| 1 | Get `EntireBible-DR.json` (or equivalent) from xxruyle/Bible-DouayRheims. |
| 2 | Run `convert_dr_to_app_json.py` → produces `bible_source.json` in app format. |
| 3 | Run `build_bible_assets.py` → produces `bible_text.bin` and `verse_index.bin`. |
| 4 | Copy output to SD `/apps_data/bible/`. |
| 5 | (Optional) Update `books_meta.c` with full verse counts. |

After Step 4, the app will serve all verses from the SD card when the card is present and assets are valid.
