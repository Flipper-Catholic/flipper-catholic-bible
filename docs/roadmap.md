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

### Known Limitations & Critical Issues
- 🔴 **CRITICAL BUG**: Reader View draw callback not rendering (blank screen) - **P0 Blocker**
- ⚠️ ~~Verse selection shows placeholder widget ("Reader text next.")~~ ✅ **Fixed in Phase 1.1**
- ⚠️ ~~No actual Bible text loading or display~~ ✅ **Partially Fixed in Phase 1.2 (Genesis 1:1-31 only)**
- ⚠️ Verse counts use stub function (defaults to 50 per chapter, except Genesis 1) - **P1**
- ⚠️ Only 31 verses implemented (0.01% of full Bible) - **P0 Blocker**
- ⚠️ Search scene is placeholder only
- ⚠️ No SD card file access (Phase 2 not started)
- ⚠️ No persistence (bookmarks, history) - Phase 4 not started
- ⚠️ No error handling for missing SD card or corrupt files

---

## Phase 1: Core Reading Experience 🎯

**Goal**: Enable reading actual Bible verses on the device.

### 1.1 Reader View Implementation
- [x] Add custom View with draw callback (replaced TextBox approach)
- [x] Implement Reader scene structure
- [x] Add navigation between verses (Left/Right arrow keys)
- [x] Add scrolling support (Up/Down arrow keys)
- [x] Add verse context display (Book Name, Chapter:Verse)
- [x] Input handling for navigation and scrolling
- 🔴 **CRITICAL BUG**: View draw callback not rendering (blank screen) - **P0 Blocker**

**Dependencies**: None  
**Priority**: Critical (blocking all other features)  
**Estimated Complexity**: High (Custom view implementation)

**Status**: Implementation complete but non-functional due to rendering bug. Draw callback may not be invoked or View setup is incorrect. Needs debugging.

**Note**: Switched from TextBox to custom View for better control over scrolling and rendering. Current implementation has draw callback that should render header, separator, and verse text, but screen remains blank.

### 1.2 Basic Text Loading
- [x] Create simple in-memory verse data structure
- [x] Implement hardcoded sample verses (Genesis 1:1-31)
- [x] Integrate sample text with Reader view
- [x] Replace placeholder verse selection with Reader navigation
- ⚠️ **Limitation**: Only Genesis 1:1-31 implemented (31 verses out of ~31,000+ total)

**Dependencies**: 1.1  
**Priority**: Critical  
**Estimated Complexity**: Low-Medium

**Status**: Partially complete. Sample verses implemented but reader view rendering bug prevents display. Full Bible text requires Phase 2 (SD card asset loading).

### 1.3 Verse Count Data
- [ ] Replace `cb_chapter_verses_stub()` with real verse counts
- [ ] Add verse count array to `books_meta.c`
- [ ] Update Browse Verses scene to show accurate verse lists
- [ ] Validate verse bounds in Reader view

**Dependencies**: 1.2  
**Priority**: High  
**Estimated Complexity**: Low (data entry)

**Phase 1 Acceptance Criteria**:
- User can browse to any verse and see actual text
- Navigation between verses works smoothly
- All 73 books are accessible via navigation

---

## Phase 2: Asset Pipeline & Storage 💾

**Goal**: Move from hardcoded text to SD card-based assets.

### 2.1 Asset Build Tool
- [ ] Create `tools/build_assets.py` (per asset-build-pipeline.md)
- [ ] Implement source text parsing (USFM/OSIS/JSON)
- [ ] Build `bible_text.bin` (verse text concatenation)
- [ ] Build `verse_index.bin` (offset/length mapping)
- [ ] Build `canon_table.bin` (book/chapter metadata)
- [ ] Generate `metadata.json` with hashes and validation

**Dependencies**: None (can start in parallel with Phase 1)  
**Priority**: High (blocks full Bible content)  
**Estimated Complexity**: High (requires external Bible text source)

### 2.2 SD Card Storage Adapter
- [ ] Implement Storage Adapter module (`storage_adapter.c/h`)
- [ ] Add SD card detection and mount handling
- [ ] Implement file reading for `bible_text.bin`
- [ ] Implement verse lookup via `verse_index.bin`
- [ ] Add integrity checking (magic numbers, version validation)
- [ ] Handle missing/corrupt file errors gracefully

**Dependencies**: 2.1 (or at least format specs)  
**Priority**: High  
**Estimated Complexity**: Medium-High

### 2.3 Integration with Reader
- [ ] Replace hardcoded verses with SD card loading
- [ ] Implement lazy loading (load verses on demand)
- [ ] Add loading spinner/indicator
- [ ] Handle out-of-bounds verse access
- [ ] Optimize memory usage (unload previous verses)

**Dependencies**: 2.1, 2.2  
**Priority**: High  
**Estimated Complexity**: Medium

**Phase 2 Acceptance Criteria**:
- Full Douay-Rheims Bible accessible from SD card
- All 73 books load correctly
- Reading performance is acceptable
- Missing SD card shows guided recovery message

---

## Phase 3: Search Functionality 🔍

**Goal**: Enable indexed full-text search across all verses.

### 3.1 Search Index Build
- [ ] Extend asset build tool to create search index
- [ ] Implement tokenization (normalize, lowercase, strip punctuation)
- [ ] Build inverted index (token → VerseID lists)
- [ ] Implement sharding (2-char prefix → shard mapping)
- [ ] Generate `shard_map.bin` and `shards/shard_*.bin`
- [ ] Update `metadata.json` with search index metadata

**Dependencies**: 2.1  
**Priority**: Medium (nice-to-have feature)  
**Estimated Complexity**: High (indexing algorithms)

### 3.2 Search UI
- [ ] Replace placeholder Search scene with TextInput view
- [ ] Implement on-screen keyboard input handling
- [ ] Add search query validation and normalization
- [ ] Create search results scene/view
- [ ] Add search history/previous queries

**Dependencies**: None (UI only)  
**Priority**: Medium  
**Estimated Complexity**: Low-Medium

### 3.3 Search Engine
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

### 4.1 Bookmark Manager
- [ ] Design bookmark storage format (Flipper app storage)
- [ ] Implement Bookmark Manager module (`bookmark_manager.c/h`)
- [ ] Add bookmark save/delete operations
- [ ] Load bookmarks on app start
- [ ] Add bookmark list UI (new scene or submenu)
- [ ] Integrate bookmark access from Reader view

**Dependencies**: Phase 1 (Reader view)  
**Priority**: Medium  
**Estimated Complexity**: Medium

### 4.2 History Manager
- [ ] Implement History Manager module (`history_manager.c/h`)
- [ ] Track last-read verse (auto-save)
- [ ] Maintain recent reading history (e.g., last 20 verses)
- [ ] Load history on app start
- [ ] Add history list UI
- [ ] Implement history cleanup (limit size)

**Dependencies**: Phase 1 (Reader view)  
**Priority**: Medium  
**Estimated Complexity**: Medium

### 4.3 UI Integration
- [ ] Add bookmark button/action in Reader view
- [ ] Add "Recent" option to main menu
- [ ] Implement bookmark editing/deletion
- [ ] Add "Jump to last read" on app launch
- [ ] Update About scene with feature credits

**Dependencies**: 4.1, 4.2  
**Priority**: Medium  
**Estimated Complexity**: Low-Medium

**Phase 4 Acceptance Criteria**:
- Bookmarks persist across app restarts
- Last-read verse is restored on launch
- History list shows recent verses correctly
- Multiple bookmarks can be managed

---

## Phase 5: Polish & Optimization ⚡

**Goal**: Improve stability, performance, and user experience.

### 5.1 Error Handling & Recovery
- [ ] Implement guided recovery mode (per system-architecture.md)
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

1. **Phase 1** (Core Reading) - Build the foundation
2. **Phase 2** (Storage) - Enable full Bible content
3. **Phase 4** (Bookmarks/History) - Add persistence (can partially parallel with Phase 3)
4. **Phase 3** (Search) - Add advanced feature
5. **Phase 5** (Polish) - Prepare for release

**Rationale**: Reading is essential before other features. Storage enables real content. Bookmarks/history can be built while search index is being developed. Search is complex and can come later. Polish comes last.

---

## Critical Blockers

1. 🔴 **Reader View Rendering Bug** (P0): View draw callback not rendering - blocks all reading functionality
2. 🔴 **Incomplete Text Content** (P0): Only 31 verses (Genesis 1:1-31) - 0.01% of full Bible
3. **Bible Text Source**: Need access to Douay-Rheims text in structured format (USFM/OSIS/JSON)
4. **Asset Build Tool**: Must be completed before full Bible content is available (Phase 2.1)
5. **Storage Adapter**: Blocks SD card-based content loading (Phase 2.2)

---

## Notes

- Phases can overlap where dependencies allow
- Test incrementally after each phase
- Consider creating feature branches for each major phase
- Document any deviations from original specs as they occur
- Keep this roadmap updated as implementation progresses
