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
   Options:
   - `--input PATH`  Input JSON (default: `assets/source/bible_source.json`)
   - `--output DIR` Output directory (default: `dist/apps_data/bible`)

3. **Copy to Flipper SD card**  
   Copy the contents of the output directory to the SD card at:
   ```
   /apps_data/bible/
   ```
   So the Flipper has:
   - `/apps_data/bible/bible_text.bin`
   - `/apps_data/bible/verse_index.bin`

4. **Add more verses**  
   - Extend `assets/source/bible_source.json` with more chapters/books.  
   - For full Douay-Rheims text, use a public-domain source (e.g. structured text from [DRBO](https://www.drbo.org/) or similar) and convert it to the JSON format, or add a separate importer in the tool.

The app will use SD card text when present and fall back to hardcoded Genesis 1 (and “verse not available”) otherwise.
