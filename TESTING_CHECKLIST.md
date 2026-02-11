# Catholic Bible App - Device Testing Checklist
**Flipper Zero Application Testing**  
**Version:** Pre-Release (Phase 1, 2, 4, 6.1, 7 Guides scaffold)  
**Date:** January 2026

---

## Test Environment Setup

### Prerequisites
- [ ] Flipper Zero device with latest stable firmware
- [ ] microSD card inserted (optional for some tests)
- [ ] Device fully charged or connected to power
- [ ] App built and installed via `ufbt launch`

### Test Notes
- **Tester Name:** ________________
- **Test Date:** ________________
- **Firmware Version:** ________________
- **App Build:** ________________
- **SD Card Present:** ☐ Yes ☐ No

---

## 1. App Launch & Basic Navigation

### 1.1 App Launch
- [ ] **TC-001**: App launches from Flipper menu without crash
- [ ] **TC-002**: App displays main menu immediately after launch
- [ ] **TC-003**: App exits cleanly when pressing Back button
- [ ] **TC-004**: App returns to Flipper menu correctly

**Expected Result:** App launches and exits without errors or crashes

**Notes:** _________________________________________________

---

### 1.2 Main Menu Display
- [ ] **TC-005**: Main menu displays all 11 menu items:
  - [ ] Browse
  - [ ] Last read
  - [ ] Search
  - [ ] Missal
  - [ ] Rosary
  - [ ] Prayers
  - [ ] Confession
  - [ ] Guides
  - [ ] Bookmarks
  - [ ] History
  - [ ] About
- [ ] **TC-006**: Menu items are readable and properly formatted
- [ ] **TC-007**: Menu navigation works (Up/Down arrow keys)
- [ ] **TC-008**: Menu selection works (OK button)
- [ ] **TC-009**: Back button returns to Flipper menu

**Expected Result:** All menu items visible and navigable

**Notes:** _________________________________________________

---

### 1.3 Browse Navigation Flow
- [ ] **TC-010**: Selecting "Browse" opens book selection
- [ ] **TC-011**: All 73 books are listed in canonical order
- [ ] **TC-012**: Book list is scrollable (Up/Down)
- [ ] **TC-013**: Selecting a book (e.g., Genesis) opens chapter selection
- [ ] **TC-014**: Chapter list shows correct chapter count for selected book
- [ ] **TC-015**: Chapter pagination works (if >40 chapters)
- [ ] **TC-016**: Selecting a chapter opens verse selection
- [ ] **TC-017**: Verse list shows correct verse count (Genesis should show accurate counts)
- [ ] **TC-018**: Selecting a verse opens reader view

- [ ] **TC-018b**: From verse list, Back button returns to chapter list (and Back from chapters returns to books, etc.)

**Expected Result:** Complete navigation flow works: Menu → Books → Chapters → Verses → Reader; Back exits each level correctly.

**Notes:** _________________________________________________

---

## 2. Reader View Functionality

### 2.1 Reader View Display
- [ ] **TC-019**: Reader view displays without blank screen
- [ ] **TC-020**: Header displays correctly: "Book Name Chapter:Verse" (e.g., "Genesis 1:1")
- [ ] **TC-021**: Header separator line is visible
- [ ] **TC-022**: Verse text displays correctly for Genesis 1:1-31
- [ ] **TC-023**: Text is readable and properly formatted
- [ ] **TC-024**: Text wraps correctly within screen boundaries
- [ ] **TC-025**: No text overflow or clipping

**Expected Result:** Reader view renders correctly with header and verse text

**Notes:** _________________________________________________

---

### 2.2 Verse Navigation (Infinite Scroll + Left/Right)
- [ ] **TC-026**: Down arrow at bottom of verse advances to next verse (infinite scroll)
- [ ] **TC-027**: Up arrow at top of verse goes to previous verse (infinite scroll)
- [ ] **TC-026b**: Right arrow key also advances to next verse
- [ ] **TC-027b**: Left arrow key also goes to previous verse
- [ ] **TC-028**: Navigation works for Genesis 1:1 through 1:31
- [ ] **TC-029**: Cannot navigate below verse 1 (boundary check)
- [ ] **TC-030**: Cannot navigate above verse 31 (boundary check)
- [ ] **TC-031**: Header updates correctly when verse changes
- [ ] **TC-032**: Verse text updates correctly when verse changes
- [ ] **TC-033**: Navigation is responsive (no lag)

**Expected Result:** Verse navigation works smoothly with proper boundary handling

**Notes:** _________________________________________________

---

### 2.3 Scrolling (Up/Down)
- [ ] **TC-034**: Down arrow key scrolls text down
- [ ] **TC-035**: Up arrow key scrolls text up
- [ ] **TC-036**: Scrolling works for long verses
- [ ] **TC-037**: Cannot scroll above top of text
- [ ] **TC-038**: Cannot scroll below bottom of text
- [ ] **TC-039**: Scroll position persists when navigating verses
- [ ] **TC-040**: Scrolling is smooth and responsive

**Expected Result:** Text scrolling works correctly within verse boundaries

**Notes:** _________________________________________________

---

### 2.4 Non-Genesis Books
- [ ] **TC-041**: Navigate to a non-Genesis book (e.g., Exodus)
- [ ] **TC-042**: Reader view displays placeholder message
- [ ] **TC-043**: Placeholder message is clear: "(Verse text not yet available. Coming in Phase 2.)"
- [ ] **TC-044**: Navigation still works (Left/Right)
- [ ] **TC-045**: Header still displays correctly (e.g., "Exodus 1:1")

**Expected Result:** Non-Genesis books show appropriate placeholder

**Notes:** _________________________________________________

---

## 3. Bookmarks Functionality

### 3.1 Bookmark Display
- [ ] **TC-046**: Selecting "Bookmarks" from main menu opens bookmark scene (submenu)
- [ ] **TC-047**: Empty state displays: "(No bookmarks)" and "OK in reader to add"
- [ ] **TC-048**: With bookmarks, list shows bookmark names; selecting one opens reader for that verse
- [ ] **TC-049**: OK in reader toggles bookmark; header shows "(BM)" when bookmarked
- [ ] **TC-050**: Back button returns to main menu

**Expected Result:** Bookmark scene displays correctly

**Notes:** _________________________________________________

---

### 3.2 Bookmark Persistence
- [ ] **TC-051**: Add a bookmark (if feature implemented in reader)
- [ ] **TC-052**: Exit app completely
- [ ] **TC-053**: Relaunch app
- [ ] **TC-054**: Navigate to Bookmarks
- [ ] **TC-055**: Bookmark persists after restart
- [ ] **TC-056**: Multiple bookmarks can be stored
- [ ] **TC-057**: Bookmarks survive power cycle

**Expected Result:** Bookmarks persist across app restarts and power cycles

**Notes:** _________________________________________________

---

## 4. History Functionality

### 4.1 History Display
- [ ] **TC-058**: Selecting "History" from main menu opens history scene (submenu "Recent")
- [ ] **TC-059**: Empty state: "(No history yet)" and "Read to build history"
- [ ] **TC-060**: With history, list shows "Book Ch:V" entries; selecting one opens reader
- [ ] **TC-061**: Back from History returns to main menu
- [ ] **TC-062**: "Last read" menu: if has last-read verse, opens reader at that verse; else opens History scene

**Expected Result:** History scene displays correctly

**Notes:** _________________________________________________

---

### 4.2 History Tracking
- [ ] **TC-063**: Navigate to Genesis 1:5 and read it
- [ ] **TC-064**: Exit reader view
- [ ] **TC-065**: Navigate to History
- [ ] **TC-066**: History shows at least 1 entry
- [ ] **TC-067**: Navigate to different verses (1:10, 1:20)
- [ ] **TC-068**: History updates automatically
- [ ] **TC-069**: Last-read verse is tracked

**Expected Result:** History tracks reading automatically

**Notes:** _________________________________________________

---

### 4.3 History Persistence
- [ ] **TC-070**: Read several verses to build history
- [ ] **TC-071**: Exit app completely
- [ ] **TC-072**: Relaunch app
- [ ] **TC-073**: Navigate to History
- [ ] **TC-074**: History persists after restart
- [ ] **TC-075**: History survives power cycle

**Expected Result:** History persists across app restarts and power cycles

**Notes:** _________________________________________________

---

## 5. Phase 6 Devotional & Phase 7 Guides

### 5.1 Missal Feature
- [ ] **TC-076**: Selecting "Missal" from main menu opens Missal submenu
- [ ] **TC-077**: Submenu displays all options:
  - [ ] Today's Mass
  - [ ] Browse by Date
  - [ ] Liturgical Calendar
  - [ ] Mass Prayers
  - [ ] Mass Responses
  - [ ] Readings Search
- [ ] **TC-078**: Today's Mass opens text view (first reading or "data not loaded" if no missal.bin)
- [ ] **TC-079**: Liturgical Calendar / Mass Prayers / Mass Responses open list, then selecting an item opens text view (when missal.bin present)
- [ ] **TC-080**: Browse by Date opens readings list; selecting a reading opens text view
- [ ] **TC-081**: Readings Search shows "Coming soon" placeholder
- [ ] **TC-082**: Back from text/list returns to Missal submenu, then to main menu

**Expected Result:** Missal menu functional with missal.bin; graceful message when absent

**Notes:** _________________________________________________

---

### 5.2 Rosary Feature
- [ ] **TC-081**: Selecting "Rosary" from main menu opens Rosary submenu
- [ ] **TC-082**: Submenu displays all options:
  - [ ] How to Pray Rosary
  - [ ] Joyful Mysteries
  - [ ] Sorrowful Mysteries
  - [ ] Glorious Mysteries
  - [ ] Luminous Mysteries
  - [ ] Rosary Prayers
  - [ ] Scriptural Rosary
  - [ ] Rosary Intentions
- [ ] **TC-083**: Selecting any option shows placeholder widget
- [ ] **TC-084**: Placeholder shows: "Rosary Feature - Coming soon. - Phase 6.2"
- [ ] **TC-085**: Back button returns to main menu

**Expected Result:** Rosary menu accessible, shows placeholder

**Notes:** _________________________________________________

---

### 5.3 Common Prayers Feature
- [ ] **TC-086**: Selecting "Prayers" from main menu opens Prayers submenu
- [ ] **TC-087**: Submenu displays all 15 prayers:
  - [ ] Sign of the Cross
  - [ ] Prayer of Holy Spirit
  - [ ] Act of Contrition
  - [ ] Lord's Prayer
  - [ ] Hail Mary
  - [ ] Glory Be
  - [ ] Guardian Angel
  - [ ] Hail Holy Queen
  - [ ] Memorare
  - [ ] St. Francis Prayer
  - [ ] St. Michael Prayer
  - [ ] Apostles' Creed
  - [ ] Prayer Before Meals
  - [ ] Morning Offering
  - [ ] Evening Prayer
- [ ] **TC-088**: Selecting any prayer opens scrollable PrayerView (from devotional.bin when present)
- [ ] **TC-089**: Back from PrayerView returns to Prayers list, then to main menu

**Expected Result:** Prayers menu shows list; selecting opens scrollable prayer text

**Notes:** _________________________________________________

---

### 5.4 Confession Preparation Feature
- [ ] **TC-091**: Selecting "Confession" from main menu opens Confession submenu
- [ ] **TC-092**: Submenu displays all options:
  - [ ] Confession Guide
  - [ ] 10 Commandments
  - [ ] 7 Deadly Sins
  - [ ] Examination Q&A
  - [ ] Act of Contrition
  - [ ] Confession Prayers
  - [ ] Tips for Nervous
  - [ ] Post-Confession
- [ ] **TC-093**: Selecting any option shows placeholder widget
- [ ] **TC-094**: Placeholder shows: "Confession Prep - Coming soon. - Phase 6.4"
- [ ] **TC-095**: Back button returns to main menu

**Expected Result:** Confession menu accessible, shows placeholder

**Notes:** _________________________________________________

---

### 5.5 Guides (Phase 7)
- [ ] **TC-G01**: Selecting "Guides" from main menu opens Guides submenu
- [ ] **TC-G02**: Submenu displays: Order of Mass, OCIA Guide, Lenten Guide, Easter Guide, Pentecost Guide, Sacraments Guide, Marrying Catholic
- [ ] **TC-G03**: Selecting any guide opens scrollable text view (placeholder or future .bin content)
- [ ] **TC-G04**: Sacraments Guide shows seven sacraments and groupings; Marrying Catholic shows marriage/preparation/audiences
- [ ] **TC-G05**: Back from guide view returns to Guides submenu, then to main menu

**Expected Result:** Guides menu and all 7 items open scrollable content

**Notes:** _________________________________________________

---

## 6. Error Handling & Edge Cases

### 6.1 Missing SD Card
- [ ] **TC-096**: Remove SD card from device
- [ ] **TC-097**: Launch app
- [ ] **TC-098**: App launches without crash
- [ ] **TC-099**: Navigation still works
- [ ] **TC-100**: Genesis 1:1-31 still displays (hardcoded fallback)
- [ ] **TC-101**: No error messages displayed (graceful degradation)
- [ ] **TC-102**: Re-insert SD card
- [ ] **TC-103**: App continues to work normally

**Expected Result:** App handles missing SD card gracefully

**Notes:** _________________________________________________

---

### 6.2 Boundary Conditions
- [ ] **TC-104**: Navigate to Genesis 1:1
- [ ] **TC-105**: Try to go left (should stay at 1:1)
- [ ] **TC-106**: Navigate to Genesis 1:31
- [ ] **TC-107**: Try to go right (should stay at 1:31)
- [ ] **TC-108**: Navigate to first book (Genesis)
- [ ] **TC-109**: Try to go to previous book (should stay at Genesis)
- [ ] **TC-110**: Navigate to last book (Revelation)
- [ ] **TC-111**: Try to go to next book (should stay at Revelation)

**Expected Result:** Boundary conditions handled correctly, no crashes

**Notes:** _________________________________________________

---

### 6.3 Invalid Navigation
- [ ] **TC-112**: Navigate to a book with 0 chapters (if any)
- [ ] **TC-113**: Navigate to a chapter with 0 verses (if any)
- [ ] **TC-114**: Try to access verse 0 or negative verse
- [ ] **TC-115**: Try to access verse beyond chapter limit
- [ ] **TC-116**: App handles invalid navigation gracefully
- [ ] **TC-117**: No crashes or freezes

**Expected Result:** Invalid navigation handled gracefully

**Notes:** _________________________________________________

---

## 7. Performance & Stability

### 7.1 Performance
- [ ] **TC-118**: App launches within 2 seconds
- [ ] **TC-119**: Menu navigation is responsive (<100ms delay)
- [ ] **TC-120**: Verse navigation is smooth (no lag)
- [ ] **TC-121**: Scrolling is smooth (no stuttering)
- [ ] **TC-122**: No progressive slowdown after extended use
- [ ] **TC-123**: Memory usage is reasonable (no leaks)

**Expected Result:** App performs well with no noticeable lag

**Notes:** _________________________________________________

---

### 7.2 Stability
- [ ] **TC-124**: Use app continuously for 10 minutes
- [ ] **TC-125**: Navigate through all menu items multiple times
- [ ] **TC-126**: Read through all Genesis 1 verses
- [ ] **TC-127**: No crashes or freezes
- [ ] **TC-128**: App remains responsive throughout
- [ ] **TC-129**: Power cycle device while app is running
- [ ] **TC-130**: App state is preserved (if applicable)

**Expected Result:** App is stable under extended use

**Notes:** _________________________________________________

---

## 8. Search Feature (Placeholder)

### 8.1 Search Scene
- [ ] **TC-131**: Selecting "Search" from main menu opens search scene
- [ ] **TC-132**: Search scene displays placeholder message
- [ ] **TC-133**: Placeholder is clear: "Search functionality coming in Phase 3"
- [ ] **TC-134**: Back button returns to main menu

**Expected Result:** Search scene shows placeholder (expected, not a bug)

**Notes:** _________________________________________________

---

## 9. About Scene

### 9.1 About Display
- [ ] **TC-135**: Selecting "About" from main menu opens about scene
- [ ] **TC-136**: About scene displays app information
- [ ] **TC-137**: Information is readable and accurate
- [ ] **TC-138**: Back button returns to main menu

**Expected Result:** About scene displays correctly

**Notes:** _________________________________________________

---

## 10. Integration Testing

### 10.1 Complete User Flow
- [ ] **TC-139**: Launch app → Browse → Genesis → Chapter 1 → Verse 1 → Read
- [ ] **TC-140**: Navigate through verses 1-10
- [ ] **TC-141**: Navigate to Bookmarks (check if bookmark was created)
- [ ] **TC-142**: Navigate to History (verify tracking)
- [ ] **TC-143**: Navigate to Phase 6 features (verify stubs work)
- [ ] **TC-144**: Exit app
- [ ] **TC-145**: Relaunch app
- [ ] **TC-146**: Verify bookmarks/history persist
- [ ] **TC-147**: Complete flow works end-to-end

**Expected Result:** Complete user flow works seamlessly

**Notes:** _________________________________________________

---

## 11. Visual & UI Testing

### 11.1 Display Quality
- [ ] **TC-148**: All text is readable on Flipper display
- [ ] **TC-149**: No text clipping or overflow
- [ ] **TC-150**: Font sizes are appropriate
- [ ] **TC-151**: Colors/contrast are adequate (if applicable)
- [ ] **TC-152**: UI elements are properly aligned
- [ ] **TC-153**: No visual glitches or artifacts

**Expected Result:** UI is clean and professional

**Notes:** _________________________________________________

---

## 12. Regression Testing

### 12.1 Previously Fixed Issues
- [ ] **TC-154**: Reader view does NOT show blank screen (was fixed in Phase 1.1)
- [ ] **TC-155**: ViewPort rendering works correctly
- [ ] **TC-156**: No BusFault crashes (was fixed earlier)
- [ ] **TC-157**: Memory allocation errors handled (NULL checks added)

**Expected Result:** Previously fixed bugs remain fixed

**Notes:** _________________________________________________

---

## Test Summary

### Test Results
- **Total Test Cases:** 157
- **Passed:** _____
- **Failed:** _____
- **Blocked:** _____
- **Not Tested:** _____

### Critical Issues Found
1. _________________________________________________
2. _________________________________________________
3. _________________________________________________

### Major Issues Found
1. _________________________________________________
2. _________________________________________________
3. _________________________________________________

### Minor Issues Found
1. _________________________________________________
2. _________________________________________________
3. _________________________________________________

### Recommendations
- _________________________________________________
- _________________________________________________
- _________________________________________________

---

## Sign-Off

**Tester Name:** ________________  
**Date:** ________________  
**Status:** ☐ Pass ☐ Fail ☐ Conditional Pass  
**Signature:** ________________

---

## Notes Section

### Additional Observations
_________________________________________________
_________________________________________________
_________________________________________________

### Device-Specific Issues
_________________________________________________
_________________________________________________
_________________________________________________

### Suggestions for Improvement
_________________________________________________
_________________________________________________
_________________________________________________

---

**End of Testing Checklist**
