# Bundled Bible assets (included in FAP)

This folder is packaged with the app (`fap_file_assets="files"` in `application.fam`).  
On first launch, the Flipper unpacks it to the SD card under the app’s assets path.  
The app uses these files when `/apps_data/bible/` is not present.

**Contents (built):**
- `bible_text.bin` – Full Douay-Rheims text (~4.3 MB)
- `verse_index.bin` – Verse index (~408 KB)

**To refresh the bundle** (e.g. after changing source or reconverting):

```bash
# From project root (requires full bible_source.json and Python)
python3 tools/build_bible_assets.py
cp dist/apps_data/bible/bible_text.bin dist/apps_data/bible/verse_index.bin files/
ufbt build
```

If you don’t have the full source yet, see **docs/full-bible-acquisition-plan.md** and **tools/README.md**.
