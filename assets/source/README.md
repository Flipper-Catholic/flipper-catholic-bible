# Bible source data

- **bible_source.json** – Default input for `tools/build_bible_assets.py`.  
  Contains Genesis 1 (31 verses) and Genesis 2 (25 verses), Douay-Rheims, public domain.

To add all verses for more books:
1. Add entries under `books[].chapters` with chapter number as string keys and arrays of verse strings.
2. Book order must match the Catholic canon used in the app (see `src/books_meta.c`).
3. Run `python3 tools/build_bible_assets.py` and copy the output to the Flipper SD card at `/apps_data/bible/`.

For the full Douay-Rheims Bible, you can obtain structured text from public domain sources and convert it to this JSON format (or extend the build script to accept other formats).
