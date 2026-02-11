# Bible and devotional source data

- **bible_source.json** – Default input for `tools/build_bible_assets.py`.  
  Sample: Genesis 1–2 (56 verses), Douay-Rheims, public domain.

**Adding more verses (or full Bible):**

1. **Option A – Full Bible (recommended)**  
   See **docs/full-bible-acquisition-plan.md**. In short:
   - Download `EntireBible-DR.json` from https://github.com/xxruyle/Bible-DouayRheims
   - Run: `python3 tools/convert_dr_to_app_json.py --input /path/to/EntireBible-DR.json --output assets/source/bible_source.json`
   - Then run `tools/build_bible_assets.py` and copy output to SD `/apps_data/bible/`

- **devotional.json** – Single source for Phase 6 (Missal, Rosary, Prayers, Confession). Placeholder with empty structure; path when deployed: `/apps_data/bible/devotional.json`. See **docs/devotional-data-design.md**.

2. **Option B – Manual**  
   Edit `bible_source.json`: add entries under `books[].chapters` with chapter number as string keys and arrays of verse strings. Book order must match the Catholic canon in `src/books_meta.c`. Then run `python3 tools/build_bible_assets.py` and copy the output to the Flipper SD at `/apps_data/bible/`.
