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
- Fully offline operation
- No prohibited hardware/network use
- Clear README and licensing
- Stable behavior under failure conditions

---

## Manual Test Checklist
- Remove SD card → launch app
- Remove search shards only → reading still works
- Navigate randomly across OT/NT/Deuterocanon
- Perform repeated searches with different terms
- Add/delete bookmarks, restart app

---

## Definition of Done (v1)
- All acceptance criteria satisfied
- No known crashers
- Search and navigation stable
- Documentation complete
- Ready for Flipper app catalog submission
