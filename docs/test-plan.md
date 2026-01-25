# Test Plan & Acceptance Criteria
Catholic Bible App for Flipper Zero

This document defines how correctness, stability, and readiness are verified for v1.

---

## Test Scope

### In Scope
- Asset detection and recovery
- Canon loading and navigation
- Verse rendering and paging
- Bookmarks and history
- Indexed search (offline)
- Performance and stability
- Catalog readiness

### Out of Scope
- Commentary or notes
- Cross-references
- Lectionary
- Audio or TTS
- Networking

---

## Test Environments
- Physical Flipper Zero device
- microSD card (varied brands/sizes)
- Latest stable Flipper firmware

---

## Functional Acceptance Criteria

### Installation & Launch
- App launches from menu without crash
- Exits cleanly back to menu
- Displays guided message if SD missing

### Asset Validation
- Missing `/apps_data/bible/` triggers recovery mode
- Missing or corrupt file identified by name
- Retry works after fixing SD contents

### Canon & Navigation
- All Catholic books listed in canonical order
- Book → Chapter → Verse navigation correct
- Genesis 1:1 and final verse accessible
- Boundary navigation does not overflow/underflow

### Reading & Paging
- Verse text displays correctly
- Paging uses fixed wrapped-line pages
- Paging flows across verse boundaries smoothly

### Bookmarks & History
- Multiple bookmarks persist across restarts
- Recent history list updates correctly
- Last-read verse restored on launch

---

## Search Acceptance Criteria

### Input & Matching
- On-screen keyboard accepts input
- Search is case-insensitive
- Diacritics normalized for search
- Prefix-based partial-word matching works

### Logic & Results
- AND logic default
- OR logic toggle works
- Results are verse-only
- Jumping to result opens correct verse

### Ordering & Highlighting
- Canonical ordering works
- Relevance ordering works
- Matched substrings highlighted in verse view

---

## Performance Criteria
- Typical searches complete within ~2 seconds
- UI remains responsive (spinner allowed)
- No progressive slowdown after repeated searches

---

## Reliability Criteria
- No crashes during extended navigation/search use
- Power cycling does not corrupt user state
- Corrupt search shards disable search gracefully

---

## Catalog Readiness Criteria
- Fully offline operation ✅
- No prohibited hardware/network use ✅
- Clear README and licensing ✅
- Stable behavior under failure conditions ⚠️ (basic handling, needs testing)

---

## Testing Checkpoint: Current State

**Recommended Testing Point**: ✅ **NOW**

**What's Ready for Testing:**
- Core navigation (Menu → Books → Chapters → Verses)
- Reader view rendering (ViewPort approach)
- Verse display and navigation (Genesis 1:1-31)
- Scrolling within verses (Up/Down)
- Verse navigation (Left/Right)
- Bookmarks (add, delete, persist)
- History (automatic tracking, last-read)
- Phase 6 menu navigation (Missal, Rosary, Prayers, Confession)
- SD card detection and error handling

**What's Not Ready:**
- Full Bible content (only Genesis 1)
- Search functionality (placeholder)
- Phase 6 content (stubs only)

**Testing Priority:**
1. **Critical**: Reader view rendering (verify ViewPort works)
2. **Critical**: Navigation flows (all menu paths)
3. **High**: Bookmarks persistence
4. **High**: History tracking
5. **Medium**: Error handling (missing SD card)
6. **Low**: Phase 6 stub navigation

---

## Manual Test Checklist (Current State)

### Core Functionality
- [ ] App launches without crash
- [ ] Menu displays all 9 items correctly
- [ ] Navigate: Menu → Browse → Books → Chapters → Verses
- [ ] Reader view displays header (Book Chapter:Verse)
- [ ] Reader view displays verse text (Genesis 1:1-31)
- [ ] Scrolling works (Up/Down in reader)
- [ ] Verse navigation works (Left/Right)
- [ ] Boundary handling (can't go below verse 1 or above 31)

### Bookmarks & History
- [ ] View bookmarks list (from menu)
- [ ] Bookmarks persist after app restart
- [ ] History tracks automatically when reading
- [ ] Last-read verse saved

### Error Handling
- [ ] Remove SD card → app handles gracefully
- [ ] Navigate to non-Genesis book → shows placeholder
- [ ] Navigate to invalid verse → handled gracefully

### Phase 6 Stubs
- [ ] Missal menu item accessible
- [ ] Rosary menu item accessible
- [ ] Prayers menu item accessible
- [ ] Confession menu item accessible
- [ ] Submenus display correctly

---

## Definition of Done (v1)
- All acceptance criteria satisfied
- No known crashers
- Search and navigation stable
- Documentation complete
- Ready for Flipper app catalog submission

**Current Status**: ⚠️ **Partial** - Core features testable, but full Bible content and search pending.
