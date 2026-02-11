# Changelog

All notable changes to the Catholic Bible app for Flipper Zero.

---

v0.2:
- Full Douay-Rheims Bible (73 books, 34,827 verses) bundled in FAP; no manual SD copy required for full text.
- Fallback: app reads from SD `/apps_data/bible/` if present, else uses bundled assets.
- Reader: fixed separator line overlapping first verse line; Back from reader exits to Chapter list.
- Last read menu; OK in reader toggles bookmark; Bookmarks and History submenus open verse in reader.
- Build: `bible_source.json` search in assets/source/, project root, assets/, cwd.
- Devotional: single-file design (devotional.json) and placeholder; Missal A/B/C, USCCB noted in docs.
- Docs: full-bible acquisition plan, devotional-data-design, catalog submission checklist on roadmap.

v0.1:
- Initial release: Menu → Books → Chapters → Verses navigation (73 books).
- ViewPort reader with infinite scroll; Genesis 1:1-31 hardcoded; SD card support for bible_text.bin + verse_index.bin.
- Bookmarks and History (persistent); Back navigation from Verses/Chapters/Books.
- Placeholder scenes for Search, Missal, Rosary, Prayers, Confession, About.
- Custom icon; installs under Apps → Tools.
