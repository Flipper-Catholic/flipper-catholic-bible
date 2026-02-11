# Build Tools

## Building SD card assets (all verses)

The app can load full Bible text from the SD card. To build the asset files:

1. **Edit or add source data**  
   - Default source: `assets/source/bible_source.json`  
   - Format: `{"books": [{"name": "Genesis", "chapters": {"1": ["verse1", "verse2", ...], "2": [...]}}]}`  
   - Add more books/chapters by extending the JSON (book order must match Catholic canon in `books_meta.c`).

2. **Run the build script**
   ```bash
   python3 tools/build_bible_assets.py
   ```
   If you omit `--input`, the script looks for `bible_source.json` in: `assets/source/`, project root, `assets/`, then current directory (so a misplaced file is often still found).
   Options:
   - `--input PATH`  Input JSON (default: search then `assets/source/bible_source.json`)
   - `--output DIR` Output directory (default: `dist/apps_data/bible`)

3. **Copy to Flipper SD card**  
   Copy the contents of the output directory to the SD card at:
   ```
   /apps_data/bible/
   ```
   So the Flipper has:
   - `/apps_data/bible/bible_text.bin`
   - `/apps_data/bible/verse_index.bin`

4. **Full Bible (all verses)**  
   See **docs/full-bible-acquisition-plan.md**. Use the converter to turn a full Douay-Rheims JSON into app format:
   ```bash
   # Download EntireBible-DR.json from https://github.com/xxruyle/Bible-DouayRheims
   python3 tools/convert_dr_to_app_json.py --input /path/to/EntireBible-DR.json --output assets/source/bible_source.json
   python3 tools/build_bible_assets.py
   ```
   Then copy `dist/apps_data/bible/*` to SD `/apps_data/bible/`.

The app will use SD card text when present and fall back to hardcoded Genesis 1 (and “verse not available”) otherwise.
