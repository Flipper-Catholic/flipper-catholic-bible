# Testing Quick Start Guide
**Catholic Bible App - Device Testing**

---

## Pre-Testing Checklist

Before starting device testing, ensure:

- [ ] App is built successfully (`ufbt launch` works)
- [ ] Flipper Zero is connected and accessible
- [ ] Flipper firmware is up to date
- [ ] microSD card is inserted (optional, but recommended)
- [ ] `TESTING_CHECKLIST.md` is available (printed or on screen)
- [ ] Pen/pencil for marking checklist (if printed)

---

## Quick Test Sequence (15 minutes)

### 1. Basic Functionality (5 min)
1. Launch app from Flipper menu
2. Navigate: Menu → Browse → Genesis → Chapter 1 → Verse 1
3. Verify reader view displays (not blank screen)
4. Test navigation: Right arrow (next verse), Left arrow (previous verse)
5. Test scrolling: Up/Down arrows
6. Exit app

**Expected:** All basic navigation works, reader view renders correctly

---

### 2. Bookmarks & History (3 min)
1. Launch app
2. Navigate to Bookmarks (should show "No bookmarks yet")
3. Navigate to History (should show "No history yet" or count if you read verses)
4. Navigate to Genesis 1:5 and read it
5. Check History again (should show at least 1 entry)
6. Exit and relaunch app
7. Check if History persists

**Expected:** History tracks automatically, persists across restarts

---

### 3. Search, Phase 6 & Guides (4 min)
1. **Search** → Opens static placeholder only ("Full-text search is not enabled in this build"). No text input; no crash.
2. From main menu, test:
   - **Missal** → Submenu → Today's Mass / Liturgical Calendar / Mass Prayers / Mass Responses / Browse by Date open list or text (missal.bin); Readings Search = "Coming soon"
   - **Rosary** → Submenu → Select any option → Placeholder
   - **Prayers** → List of prayers → Select one → Scrollable PrayerView (devotional.bin)
   - **Confession** → Submenu → Select any option → Placeholder
   - **Guides** → Submenu (Order of Mass, OCIA, Lenten, Easter, Pentecost, Sacraments, Marrying Catholic) → Select any → Scrollable guide text

**Expected:** Search shows placeholder only; Missal and Prayers show content when .bin present; Guides show scaffold text; Rosary/Confession show placeholders

---

### 4. Error Handling (2 min)
1. Remove SD card (if present)
2. Launch app
3. Verify app still works (Genesis 1:1-31 should still display)
4. Re-insert SD card
5. Verify app continues to work

**Expected:** App handles missing SD card gracefully

---

### 5. Boundary Testing (2 min)
1. Navigate to Genesis 1:1
2. Try to go left (should stay at 1:1)
3. Navigate to Genesis 1:31
4. Try to go right (should stay at 1:31)

**Expected:** Boundary conditions handled correctly, no crashes

---

## Full Testing

For comprehensive testing, use `TESTING_CHECKLIST.md` which contains:
- **157 test cases** organized by feature
- Detailed test steps
- Expected results
- Notes sections for observations

**Estimated Time:** 2-3 hours for complete testing

---

## Common Issues to Watch For

### Critical Issues
- ❌ **Blank screen in reader view** - This was fixed, but verify it doesn't occur
- ❌ **App crashes** - Note any crashes with steps to reproduce
- ❌ **Memory leaks** - App becomes slow after extended use

### Minor Issues
- ⚠️ **Text clipping** - Text cut off at edges
- ⚠️ **Navigation lag** - Delayed response to button presses
- ⚠️ **Display formatting** - Text not properly formatted

---

## Reporting Issues

When reporting issues, include:

1. **Test Case ID** (e.g., TC-019)
2. **Steps to Reproduce**
3. **Expected Result**
4. **Actual Result**
5. **Screenshots** (if possible)
6. **Firmware Version**
7. **App Build Info**

---

## Testing Priority

### Must Test (Before Release)
- [ ] App launch and exit
- [ ] Reader view rendering
- [ ] Verse navigation (Left/Right)
- [ ] Text scrolling (Up/Down)
- [ ] Bookmarks persistence
- [ ] History tracking
- [ ] Error handling (missing SD card)

### Should Test (Before Release)
- [ ] All menu navigation (including Guides, Last read)
- [ ] Phase 6 (Missal, Prayers, Rosary, Confession) and Phase 7 Guides
- [ ] Boundary conditions
- [ ] Extended use (10+ minutes)

### Nice to Test (If Time Permits)
- [ ] All 157 test cases
- [ ] Performance under stress
- [ ] Visual polish
- [ ] Edge cases

---

## Next Steps After Testing

1. **Document Results**
   - Mark all test cases as Pass/Fail/Blocked
   - Note any issues found
   - Complete test summary section

2. **Fix Critical Issues**
   - Address any P0/P1 bugs found
   - Re-test after fixes

3. **Update Documentation**
   - Update STATUS_REPORT.md with test results
   - Update README.md if needed
   - Document known issues

4. **Decide on Release**
   - Beta/Early Access: If minor issues only
   - Full Release: If all critical tests pass
   - Hold: If critical bugs found

---

## Quick Commands

```bash
# Build and launch app
ufbt launch

# Check app logs (if available)
# (Check Flipper CLI or serial output)

# Rebuild after code changes
ufbt clean
ufbt launch
```

---

**Ready to test?** Start with the Quick Test Sequence above, then proceed to full testing with `TESTING_CHECKLIST.md`.

Good luck! 🧪
