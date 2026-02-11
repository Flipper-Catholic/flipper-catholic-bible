# Implementation Roadmap
Catholic Bible App for Flipper Zero

This roadmap maps the features described in the documentation to implementation phases, showing current status, dependencies, and sequencing.

---

## Current Status (Phase 0) ✅

### Implemented
- ✅ Basic navigation structure (Menu → Books → Chapters → Verses)
- ✅ Scene management using Flipper SDK
- ✅ Book metadata (73 books with chapter counts)
- ✅ Chapter pagination (40 chapters per page)
- ✅ Placeholder scenes for Search and About
- ✅ Event encoding/decoding for chapters and verses
- ✅ MIT License file
- ✅ Comprehensive documentation (6 docs in `/docs`)

### Known Limitations
- ✅ ~~Reader blank screen~~ **Fixed** (ViewPort-only, infinite scroll, layout)
- ✅ ~~Verse selection / text display~~ **Fixed** (Genesis 1:1-31 in-app; Genesis 1–2 via SD)
- ✅ SD card storage and asset build tool implemented (Phase 2.1/2.2)
- ✅ Bookmarks, history, Back-from-Verses working
- ⚠️ Verse counts: Genesis complete; other books use placeholder (50)
- ⚠️ Search scene placeholder only (Phase 3)
- ⚠️ Error handling for missing/corrupt SD remains basic

---

## Phase 1: Core Reading Experience 🎯

**Goal**: Enable reading actual Bible verses on the device.

### 1.1 Reader View Implementation ✅
- [x] ViewPort-only reader (add/remove in scene – fixes blank screen)
- [x] Infinite scroll: Up/Down at top/bottom = previous/next verse; Left/Right also
- [x] Word wrap, single-spaced text, full-width lines, compact header/divider
- [x] Back exits reader; Back from Verses/Chapters/Books via submenu previous callback

**Status**: Complete. Reader renders correctly; navigation and Back working.

### 1.2 Basic Text Loading ✅
- [x] Hardcoded Genesis 1:1-31; SD card loading when assets present (Genesis 1–2 via build tool)
- [x] Reader displays text; fallback to hardcoded when no SD assets

**Status**: Complete. Genesis 1–2 (56 verses) available with SD assets; 31 verses in-app only.

### 1.3 Verse Count Data ✅
- [x] Verse count array in `books_meta.c` for all 73 books (from bible_source.json)
- [x] `tools/export_verse_counts.py` generates C array; Browse Verses shows accurate counts

**Dependencies**: 1.2  
**Status**: Complete.

**Phase 1 Acceptance Criteria**:
- User can browse to any verse and see actual text
- Navigation between verses works smoothly
- All 73 books are accessible via navigation

---

## Phase 2: Asset Pipeline & Storage 💾

**Goal**: Move from hardcoded text to SD card-based assets.

### 2.1 Asset Build Tool ✅ (partial)
- [x] `tools/build_bible_assets.py` – reads JSON, writes `bible_text.bin` + `verse_index.bin`
- [x] `assets/source/bible_source.json` – Genesis 1–2 (56 verses) Douay-Rheims
- [ ] Optional: `canon_table.bin`, `metadata.json` (not required for current reader)
- [ ] Full Bible: add more books/chapters to JSON or support other source formats

**Status**: Tool and sample data in place; copy output to SD `/apps_data/bible/`.

### 2.2 SD Card Storage Adapter ✅
- [x] Implement Storage Adapter module (`storage_adapter.c/h`)
- [x] Add SD card detection and mount handling
- [x] Implement file reading for `bible_text.bin`
- [x] Implement verse lookup via `verse_index.bin`
- [x] Add integrity checking (magic numbers, version validation)
- [x] Handle missing/corrupt file errors gracefully

**Status**: Complete. Index loaded on demand; verse text read from SD.

### 2.3 Integration with Reader ✅
- [x] Replace hardcoded verses with SD card loading (prefer SD when available)
- [x] Implement lazy loading (index loaded on first verse request)
- [ ] Add loading spinner/indicator (optional)
- [x] Handle out-of-bounds verse access (fallback message)
- [x] Optimize memory usage (single verse buffer, index cache)

**Status**: Complete. Reader uses storage adapter with hardcoded fallback.

**Phase 2 Acceptance Criteria**:
- Full Douay-Rheims Bible accessible from SD card
- All 73 books load correctly
- Reading performance is acceptable
- Missing SD card shows guided recovery message

---

## Phase 3: Search Functionality 🔍

**Goal**: Enable indexed full-text search across all verses.

### 3.1 Search Index Build ✅
- [x] `tools/build_search_index.py` – tokenize from bible_source.json, sharded inverted index
- [x] Sharding by 2-char prefix; `search_shard_map.bin` + `search_shards/shard_*.bin`

**Dependencies**: 2.1  
**Status**: Complete.

### 3.2 Search UI ✅
- [x] Search scene uses TextInput when index available; SearchResults submenu; tap opens reader

**Dependencies**: None (UI only)  
**Status**: Complete.

### 3.3 Search Engine ✅
- [x] `search_adapter.c`: load shard by prefix, token lookup (prefix match), return verse_ids; `storage_adapter_get_ref_from_verse_id` for refs

**Status**: Complete.

### 3.3 (legacy) Search Engine (detail)
- [ ] Implement Search Engine module (`search_engine.c/h`)
- [ ] Load only required shards (lazy loading)
- [ ] Implement prefix matching
- [ ] Implement AND/OR logic
- [ ] Return ranked or canonical-ordered results
- [ ] Handle large result sets (pagination)

**Dependencies**: 3.1, 3.2  
**Priority**: Medium  
**Estimated Complexity**: High

### 3.4 Result Navigation
- [ ] Display search results as verse references
- [ ] Implement result highlighting in Reader view
- [ ] Add "jump to verse" from results
- [ ] Handle empty results gracefully

**Dependencies**: 3.3  
**Priority**: Medium  
**Estimated Complexity**: Medium

**Phase 3 Acceptance Criteria**:
- Search completes within ~2 seconds for typical queries
- Partial-word matching works correctly
- AND/OR logic functions as specified
- Results navigate to correct verses
- Search works offline with SD card assets only

---

## Phase 4: Bookmarks & History 📖

**Goal**: Add persistence for user's reading progress and favorites.

### 4.1 Bookmark Manager ✅
- [x] Design bookmark storage format (Flipper app storage)
- [x] Implement Bookmark Manager module (`bookmark_manager.c/h`)
- [x] Add bookmark save/delete operations
- [x] Load bookmarks on app start
- [x] Add bookmark list UI (submenu; select opens reader)
- [x] Integrate bookmark access from Reader view (OK = toggle bookmark)

**Status**: Complete.

### 4.2 History Manager ✅
- [x] Implement History Manager module (`history_manager.c/h`)
- [x] Track last-read verse (auto-save)
- [x] Maintain recent reading history (last 20 verses)
- [x] Load history on app start
- [x] Add history list UI (submenu; select opens reader)
- [x] Implement history cleanup (limit size)

**Status**: Complete. History persisted on reader exit.

### 4.3 UI Integration ✅
- [x] Add bookmark action in Reader view (OK = toggle; header shows "(BM)" when bookmarked)
- [x] Add "Last read" option to main menu (jumps to last-read verse or History scene)
- [x] Bookmarks submenu: select verse opens reader
- [x] History submenu: select verse opens reader
- [ ] Add "Jump to last read" on app launch (optional; menu suffices)
- [x] Update About scene with SD path and features

**Status**: Complete.

**Phase 4 Acceptance Criteria**:
- Bookmarks persist across app restarts
- Last-read verse is restored on launch
- History list shows recent verses correctly
- Multiple bookmarks can be managed

---

## Phase 5: Polish & Optimization ⚡

**Goal**: Improve stability, performance, and user experience.

### 5.1 Error Handling & Recovery ✅
- [x] Try bundled assets when SD absent (no-SD devices get full Bible from FAP)
- [x] Guided message when no assets: menu header "(No data: reinstall or add SD)"; verse placeholder suggests reinstall or SD path
- [ ] Add detailed error messages for missing assets
- [ ] Handle corrupt file scenarios gracefully
- [ ] Add asset validation on startup
- [ ] Provide recovery instructions to users

**Dependencies**: Phase 2 (Storage Adapter)  
**Priority**: High (catalog readiness)  
**Estimated Complexity**: Medium

### 5.2 Performance Optimization
- [ ] Profile memory usage during navigation
- [ ] Optimize verse text loading (caching strategy)
- [ ] Reduce search index memory footprint
- [ ] Optimize UI rendering (minimize redraws)
- [ ] Test with various SD card types/sizes

**Dependencies**: All previous phases  
**Priority**: Medium  
**Estimated Complexity**: Medium-High

### 5.3 Edge Cases & Testing
- [ ] Test boundary conditions (first/last verses)
- [ ] Test with missing search shards (reading should still work)
- [ ] Test power cycling during operations
- [ ] Verify all 73 books load correctly
- [ ] Test navigation across OT/NT/Deuterocanon boundaries

**Dependencies**: All previous phases  
**Priority**: High  
**Estimated Complexity**: Medium

### 5.4 Documentation & Packaging
- [ ] Update README with installation instructions
- [ ] Document asset preparation process
- [ ] Create user guide (how to add Bible text assets)
- [ ] Prepare for Flipper app catalog submission
- [ ] Add license information and credits

**Dependencies**: All phases  
**Priority**: Medium (for catalog submission)  
**Estimated Complexity**: Low

**Phase 5 Acceptance Criteria**:
- All test plan criteria satisfied (per test-plan.md)
- No crashes during extended use
- Performance meets specified criteria
- Ready for catalog submission

---

## Dependencies Graph

```
Phase 1 (Reading)
  ├─> 1.1 Reader View
  ├─> 1.2 Basic Text Loading (depends on 1.1)
  └─> 1.3 Verse Counts (depends on 1.2)

Phase 2 (Storage)
  ├─> 2.1 Asset Build Tool (parallel)
  ├─> 2.2 Storage Adapter (depends on 2.1 specs)
  └─> 2.3 Integration (depends on 2.1, 2.2)

Phase 3 (Search)
  ├─> 3.1 Index Build (depends on 2.1)
  ├─> 3.2 Search UI (parallel)
  ├─> 3.3 Search Engine (depends on 3.1, 3.2)
  └─> 3.4 Results (depends on 3.3)

Phase 4 (Persistence)
  ├─> 4.1 Bookmarks (depends on Phase 1)
  ├─> 4.2 History (depends on Phase 1)
  └─> 4.3 UI Integration (depends on 4.1, 4.2)

Phase 5 (Polish)
  └─> All depends on Phases 1-4
```

---

## Recommended Implementation Sequence

1. ~~**Phase 1** (Core Reading)~~ ✅ Done
2. ~~**Phase 2** (Storage + full Bible bundle)~~ ✅ Done
3. ~~**Phase 4** (Bookmarks/History)~~ ✅ Done
4. **Catalog readiness** – Add `changelog.md`, app screenshots (qFlipper), then submit `manifest.yml` to Apps Catalog (see checklist above)
5. **Phase 5** (Polish) – Optional before/after catalog: 5.1 guided recovery message, 5.2–5.4 as needed
6. **Phase 3** (Search) or **Phase 6** (Devotional content) – As roadmap priority (P2)
7. **Phase 1.3 remainder** – Verse counts for remaining 72 books (data entry; improves lists when SD not used)

**Rationale**: Core reading, storage, and persistence are complete; full Bible is bundled. Next: meet catalog checklist (changelog, screenshots), then optional polish or new features.

---

## Critical Blockers (Resolved / Current)

1. ~~**Reader View Rendering Bug**~~ ✅ Fixed (ViewPort-only reader)
2. ~~**Storage Adapter**~~ ✅ Implemented (Phase 2.2)
3. ~~**Asset Build Tool**~~ ✅ Implemented (Phase 2.1 partial; Genesis 1–2)
4. ~~**Incomplete Text Content**~~ **Addressed**: Full Bible (34,827 verses) bundled in FAP; also loadable from SD `/apps_data/bible/`. Converter + build tool in repo.
5. **Search**: Phase 3 placeholder only (index build + engine pending)
6. **Error handling**: Basic; guided recovery (Phase 5.1) optional for catalog

---

## App catalog submission (Flipper Apps Catalog)

Requirements from [Contributing Guide](https://github.com/flipperdevices/flipper-application-catalog/blob/main/documentation/Contributing.md):

### General terms (we comply)
- [x] **Open source license** – MIT
- [x] **No infringement** – Public domain Bible (Douay-Rheims); no trademarks
- [x] **No malicious/illegal code** – Offline only; no firmware bypass
- [x] **UGC policies** – No user-generated content; static religious text

### Source repo checklist (before PR to catalog)
- [x] **Buildable with uFBT** – Compatible with current firmware
- [x] **App icon** – `icon.png` (fap_icon); catalog may expect 10×10px 1-bit for listing
- [ ] **App screenshots** – Take with qFlipper; do not change resolution; first screenshot = app preview. **Add to repo before catalog PR.**
- [x] **README.md** – Usage, installation, hardware (SD card)
- [ ] **changelog.md** – Version history in format below. **Add to repo.**
- [x] **application.fam** – appid, name, fap_category, fap_version, fap_description, fap_author

### Catalog PR (separate step)
- [ ] **manifest.yml** – Create and submit via PR to `flipperdevices/flipper-application-catalog` (applications/&lt;category&gt;/&lt;appid&gt;/manifest.yml); set `commit_sha` to this repo’s commit after changelog + screenshots are in.

### Priority order for next work (groomed)
- **Do first:** Phase 1.3 verse counts (72 books), then Phase 3 Search or Phase 6 devotional content.
- **Do last:** App screenshots (qFlipper) → then submit `manifest.yml` to Apps Catalog (catalog PR). Keep screenshots until very end so they reflect final UI.
- **Done:** changelog.md ✅; Phase 5.1 guided recovery ✅.

---

## Phase 6: Catholic Devotional Features ✝️

**Goal**: Expand app beyond Bible reading to include essential Catholic devotional resources.

**Data design:** One combined asset for Missal, Rosary, Prayers, Confession; Missal is year-dependent (A/B/C lectionary). US source: [USCCB](https://www.usccb.org/). See **docs/devotional-data-design.md**.

### 6.1 Roman Catholic Missal ✅ (wired)
- [x] Design missal data structure (readings, prayers, responses) – missal.json → missal.bin
- [x] Implement liturgical calendar (seasons); missal_loader
- [x] Add daily readings (First Reading, Psalm, Gospel) – sample keys in data
- [x] Add Mass prayers (Penitential Act, Gloria, Creed, etc.) and Mass responses
- [x] Today's Mass, Browse by Date, Liturgical Calendar, Mass Prayers/Responses list+text views
- [ ] Full date-based navigation / liturgical year (A/B/C) – future
- [ ] Support for multiple Mass forms – future

**Resources Needed**:
- Official Roman Missal text (approved translations)
- Lectionary readings (3-year cycle for Sundays, 2-year cycle for weekdays)
- Liturgical calendar data (feast days, memorials, optional memorials)
- Proper prayers for each day/feast

**Dependencies**: Phase 1 (Reader view), Phase 2 (Storage adapter)  
**Priority**: Medium  
**Estimated Complexity**: High (requires liturgical calendar and readings data)

**Features to Consider**:
- Today's Mass (automatically shows current day's readings and prayers)
- Browse by date (navigate to any date's Mass)
- Browse by liturgical season
- Search readings by keyword
- Bookmark favorite Mass prayers
- Print-friendly format (for those who want to print readings)

**Data Sources** (to be verified):
- USCCB Lectionary (United States Conference of Catholic Bishops)
- Roman Missal (official Vatican-approved translations)
- Liturgical calendar from local bishops' conference

### 6.2 Rosary Guide and Prayers
- [ ] Implement Rosary structure (20 mysteries, 5 per set)
- [ ] Add all Rosary prayers (Our Father, Hail Mary, Glory Be, Fatima Prayer, etc.)
- [ ] Create mystery navigation (Joyful, Sorrowful, Glorious, Luminous)
- [ ] Add mystery descriptions and meditations
- [ ] Implement Rosary counter/bead tracker (optional - visual aid)
- [ ] Add Scriptural Rosary (Bible verses for each Hail Mary)
- [ ] Add Rosary intentions guide
- [ ] Add how-to guide for praying the Rosary

**Resources Needed**:
- Official Rosary prayers (approved translations)
- Mystery descriptions and meditations
- Scriptural references for each mystery
- Traditional Rosary prayers (Apostles' Creed, etc.)

**Dependencies**: Phase 1 (Reader view)  
**Priority**: Medium  
**Estimated Complexity**: Medium

**Features to Consider**:
- Step-by-step Rosary guide (for beginners)
- Mystery meditations (brief reflections for each mystery)
- Scriptural Rosary (Bible verse for each bead)
- Rosary intentions (how to offer Rosary for specific needs)
- Audio guide structure (if text-to-speech becomes available)
- Multiple Rosary variations (Traditional, Scriptural, etc.)

**Data Sources** (to be verified):
- Official Catholic prayer books
- Vatican-approved Rosary guides
- Traditional Catholic sources (St. Louis de Montfort, etc.)

### 6.3 Common Prayers ✅ (initial)
- [x] `devotional.json` + `tools/build_devotional.py` → `devotional.bin` (prayers: title + text)
- [x] `devotional_loader.c`: load devotional.bin, get_prayer(index)
- [x] Prayers scene: list from loader; tap opens PrayerView (TextBox with full text); Back to list
- [ ] Expand devotional.json with more prayers; Missal/Rosary/Confession content (Phase 6.1, 6.2, 6.4)
- [ ] Add prayer search functionality
- [ ] Add prayer favorites/bookmarks
- [ ] Add prayer context (when to pray, history, etc.)

**Initial Prayer List**:
- Sign of the Cross
- Prayer of the Holy Spirit
- Act of Contrition
- Lord's Prayer (Our Father)
- Hail Mary
- Glory Be to the Father (Doxology)
- Guardian Angel Prayer
- Hail, Holy Queen (Salve Regina)
- Memorare
- Peace Prayer of St. Francis
- Prayer to St. Michael the Archangel
- Apostles' Creed
- Prayer Before Meals
- Morning Offering
- Evening Prayer

**Resources Needed**:
- Official prayer texts (approved translations)
- Prayer categories and organization
- Context for each prayer (when, why, history)

**Dependencies**: Phase 1 (Reader view), Phase 4 (Bookmarks)  
**Priority**: Medium  
**Estimated Complexity**: Low-Medium

**Features to Consider**:
- Categorized prayer list (Daily, Devotional, Saints, etc.)
- Search prayers by keyword
- Favorite prayers (quick access)
- Prayer of the day (rotating selection)
- Full prayer text with proper formatting
- Audio support (if text-to-speech becomes available)
- Multiple language support (future)

**Data Sources** (to be verified):
- Official Catholic prayer books (Roman Catholic)
- Vatican-approved prayer collections
- Traditional Catholic sources

### 6.4 Confession Preparation
- [ ] Design flash card system for Confession
- [ ] Add Confession guide (step-by-step process)
- [ ] Implement examination of conscience questions
- [ ] Add Act of Contrition variations
- [ ] Add Confession prayers and responses
- [ ] Create Q&A cards for reflection
- [ ] Add tips for nervous/returning Catholics
- [ ] Add post-Confession prayers and guidance

**Resources Needed**:
- Official Confession format and prayers
- Examination of conscience questions (10 Commandments, 7 Deadly Sins, etc.)
- Act of Contrition text (multiple approved versions)
- Confession guide (what to say, how to prepare)

**Dependencies**: Phase 1 (Reader view)  
**Priority**: Medium  
**Estimated Complexity**: Medium

**Features to Consider**:
- Step-by-step Confession guide (before, during, after)
- Examination of conscience flash cards (Q&A format)
- Confession prayers (Act of Contrition, etc.)
- What to say in Confession (script/guide)
- Tips for nervous Catholics
- Post-Confession guidance (penance, thanksgiving)
- Multiple examination formats (10 Commandments, 7 Deadly Sins, etc.)
- Age-appropriate examinations (children, teens, adults)

**Important Note**: This feature is for PREPARATION only. Users should NOT bring the Flipper device into the Confession booth. This is to help new, returning, and nervous Catholics prepare properly.

**Data Sources** (to be verified):
- Official Catholic Confession guides
- Examination of conscience from approved sources
- Traditional Catholic preparation materials
- Catechism of the Catholic Church references

---

## Notes

- Phases can overlap where dependencies allow
- Test incrementally after each phase
- Consider creating feature branches for each major phase
- Document any deviations from original specs as they occur
- Keep this roadmap updated as implementation progresses
- **Phase 6 features**: All require verified Roman Catholic sources. Ensure all texts are from approved, official sources.