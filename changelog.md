# Changelog

All notable changes to the Catholic Bible app for Flipper Zero.

---

v0.4 (unreleased):
- Phase 6.1 Missal: missal_loader + missal.bin; Today's Mass, Liturgical Calendar, Mass Prayers, Mass Responses, Browse by Date (readings); MissalList and MissalText scenes.
- Phase 7 Guides: Top-level Guides menu with 7 items (Order of Mass, OCIA, Lenten, Easter, Pentecost, Sacraments Guide, Marrying Catholic); GuideView with scaffold content; Sacraments and Marrying Catholic full placeholder text.
- Refactor: DEVOTIONAL_DISPLAY_BUF_SIZE constant for display buffer.
- Device stability: Verse index no longer loaded into RAM (avoids OOM on Browse → Chapter, History, Last read). Verse counts from books_meta.c; verse text read per-verse from bible_text.bin.
- Search: Always show placeholder screen ("Full-text search is not enabled in this build"); no TextInput to avoid crashes. Phase 3 search deferred.
- Docs: phase6-phase7-plan (Sacraments & Marrying Catholic); README/STATUS/checklists updated; testing checklist and doc housekeeping.

v0.3 (unreleased):
- Phase 1.3: Verse counts for all 73 books (from bible_source.json via export_verse_counts.py).
- Phase 3: Search – build_search_index.py (sharded index), search_adapter (lookup by token), TextInput + SearchResults scene; tap result opens reader.
- Phase 5.1: Try bundled FAP assets even when SD card is absent; guided recovery message when no assets.
- Phase 6.3: Common Prayers from devotional.bin (devotional.json + build_devotional.py); devotional_loader; Prayers list + PrayerView (TextBox).

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
