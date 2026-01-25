# Development Plan - Phase 1 Completion
**Catholic Bible App for Flipper Zero**  
**Date:** January 2025  
**Goal:** Complete Phase 1 (Core Reading Experience) in one development block

---

## Executive Summary

This plan addresses the **P0 and P1 blockers** identified in the Status Report to complete Phase 1 of the roadmap. The focus is on making the core reading experience functional, even if limited to Genesis 1 initially.

**Target Completion:** Phase 1.1, 1.2, and 1.3  
**Estimated Effort:** 1 massive development block  
**Success Criteria:** User can navigate to Genesis 1:1-31, see text rendered correctly, scroll within verses, and navigate between verses.

---

## Priority-Based Task List

### 🔴 **P0: Fix Reader View Rendering Bug** (CRITICAL)

**Problem:** Custom View draw callback not rendering - blank screen when entering reader view.

**Root Cause Analysis:**
- View is registered with ViewDispatcher
- Draw callback is set
- Context is set
- But screen remains blank (even "TEST" string not visible)

**Investigation Strategy:**
1. **Try ViewPort approach** (more reliable for custom drawing)
   - Create ViewPort directly
   - Use `view_port_draw_callback_set()`
   - Use `view_port_input_callback_set()` (returns void, not bool)
   - Wrap ViewPort in View using `view_alloc()` and `view_set_view_port()`

2. **Verify View lifecycle**
   - Ensure View is enabled
   - Check if view needs explicit update call
   - Verify ViewDispatcher is properly switching to view

3. **Compare with working Widget implementation**
   - Widget uses ViewPort internally
   - May need to follow same pattern

**Implementation Steps:**
- [ ] Create ViewPort-based reader view
- [ ] Set draw callback on ViewPort
- [ ] Set input callback on ViewPort (note: void return type)
- [ ] Wrap ViewPort in View
- [ ] Register with ViewDispatcher
- [ ] Test rendering with simple "TEST" string
- [ ] Once rendering works, implement full text drawing

**Fallback:** If ViewPort approach fails, try using Widget with custom drawing or investigate View API usage more deeply.

---

### 🟡 **P1: Complete Verse Count Data** (HIGH PRIORITY)

**Problem:** `cb_chapter_verses_stub()` returns 50 for all chapters except Genesis 1.

**Solution:** Add real verse count data to `books_meta.c`

**Implementation Steps:**
- [ ] Create verse count array structure
- [ ] Add verse counts for all 73 books
- [ ] Replace stub function with real data lookup
- [ ] Add bounds checking in navigation
- [ ] Test with Genesis 1 (should show 31 verses)
- [ ] Test with other books (should show accurate counts)

**Data Structure:**
```c
// In books_meta.h
extern const uint16_t catholic_bible_verse_counts[CATHOLIC_BIBLE_BOOKS_COUNT][MAX_CHAPTERS];

// Or more efficient: flattened array with chapter offsets
// Or: array of pointers to chapter verse count arrays
```

**Note:** This is data entry work. Can use online Bible reference for verse counts. Focus on accuracy for Genesis first, then expand.

---

### 🧹 **Code Cleanup & Improvements**

**Tasks:**
- [ ] Remove "TEST" debug string from production code
- [ ] Improve `reader_draw_text()` function
  - Better word wrapping algorithm
  - Handle edge cases (very long words, special characters)
  - Optimize rendering performance
- [ ] Add bounds checking for verse navigation
- [ ] Improve error handling (NULL checks, bounds validation)
- [ ] Add comments for complex logic

---

## Implementation Sequence

### Phase 1A: Fix Rendering (P0) - ~60% of effort
1. **Switch to ViewPort approach**
   - Replace View-based reader with ViewPort-based
   - Test with simple drawing first
   - Verify callback invocation

2. **Implement full text rendering**
   - Once basic rendering works, add verse text
   - Test with Genesis 1:1-31
   - Verify scrolling works

3. **Test and debug**
   - Navigate to verse
   - Verify text displays
   - Test Up/Down scrolling
   - Test Left/Right navigation

### Phase 1B: Verse Counts (P1) - ~20% of effort
1. **Design data structure**
   - Choose efficient storage format
   - Add to `books_meta.h` and `books_meta.c`

2. **Add verse count data**
   - Start with Genesis (all 50 chapters)
   - Expand to other books as needed
   - Focus on accuracy

3. **Replace stub function**
   - Update `cb_chapter_verses_stub()` to use real data
   - Add bounds checking
   - Test navigation

### Phase 1C: Polish & Cleanup - ~20% of effort
1. **Remove debug code**
   - Remove "TEST" strings
   - Clean up commented code

2. **Improve text rendering**
   - Better word wrapping
   - Handle edge cases
   - Performance optimization

3. **Add error handling**
   - NULL pointer checks
   - Bounds validation
   - Graceful error messages

---

## Technical Approach: ViewPort vs View

### Current Approach (Not Working)
```c
app->reader_view = view_alloc();
view_set_draw_callback(app->reader_view, reader_view_draw_callback);
view_set_input_callback(app->reader_view, reader_view_input_callback);
```

### Proposed Approach (ViewPort)
```c
// Create ViewPort
app->reader_viewport = view_port_alloc();
view_port_draw_callback_set(app->reader_viewport, reader_viewport_draw_callback, app);
view_port_input_callback_set(app->reader_viewport, reader_viewport_input_callback, app);
view_port_enabled_set(app->reader_viewport, true);

// Wrap in View
app->reader_view = view_alloc();
view_set_view_port(app->reader_view, app->reader_viewport);
view_set_context(app->reader_view, app);

// Register with dispatcher
view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewReader, app->reader_view);
```

**Key Differences:**
- ViewPort has separate draw/input callbacks
- ViewPort input callback returns `void` (not `bool`)
- ViewPort is wrapped in View for ViewDispatcher compatibility
- This matches how Widget works internally

---

## Data Structure for Verse Counts

### Option 1: 2D Array (Simple but memory-intensive)
```c
#define MAX_CHAPTERS 150  // Psalms has 150 chapters
const uint16_t catholic_bible_verse_counts[CATHOLIC_BIBLE_BOOKS_COUNT][MAX_CHAPTERS];
```
**Pros:** Simple lookup  
**Cons:** Wastes memory (most books have <50 chapters)

### Option 2: Array of Pointers (Memory efficient)
```c
const uint16_t* const catholic_bible_verse_counts[CATHOLIC_BIBLE_BOOKS_COUNT];
// Each pointer points to chapter_count-sized array
```
**Pros:** Memory efficient  
**Cons:** More complex initialization

### Option 3: Flattened Array with Offsets (Balanced)
```c
// Store as single array with book/chapter offsets
// Lookup: offset = book_offset[book_index] + chapter
```
**Pros:** Memory efficient, simple lookup  
**Cons:** Requires offset calculation

**Recommendation:** Start with Option 1 for simplicity, optimize later if needed.

---

## Testing Strategy

### Unit Testing (Manual)
1. **Rendering Test**
   - Navigate to Genesis 1:1
   - Verify header displays: "Genesis 1:1"
   - Verify verse text displays
   - Verify "TEST" string removed

2. **Scrolling Test**
   - Navigate to long verse (e.g., Genesis 1:2)
   - Press Up/Down
   - Verify text scrolls
   - Verify scroll boundaries work

3. **Navigation Test**
   - Navigate Genesis 1:1 → 1:2 → 1:3
   - Navigate Genesis 1:31 → 1:30 (backwards)
   - Verify verse bounds (can't go below 1 or above 31)

4. **Verse Count Test**
   - Navigate to Genesis 1
   - Verify verse list shows 1-31 (not 1-50)
   - Navigate to other books
   - Verify accurate verse counts

### Integration Testing
- Full navigation flow: Menu → Books → Chapters → Verses → Reader
- Verify all scenes transition correctly
- Verify no crashes or memory leaks

---

## Risk Assessment

### High Risk
- **ViewPort approach may not work** - Fallback to investigating View API more deeply
- **Verse count data entry errors** - Need careful verification

### Medium Risk
- **Text rendering performance** - May need optimization for long verses
- **Memory usage** - Verse count array may be large

### Low Risk
- **Code cleanup** - Straightforward
- **Error handling** - Defensive programming

---

## Success Metrics

### Phase 1 Complete When:
- ✅ Reader view renders text correctly (no blank screen)
- ✅ User can navigate to Genesis 1:1-31
- ✅ Text displays with proper word wrapping
- ✅ Scrolling works (Up/Down)
- ✅ Verse navigation works (Left/Right)
- ✅ Verse counts are accurate (at least for Genesis)
- ✅ No crashes during normal use
- ✅ Code is clean (no debug strings)

### Phase 1.3 Complete When:
- ✅ All 73 books have accurate verse counts
- ✅ Navigation shows correct verse lists
- ✅ Bounds checking prevents invalid verse access

---

## Estimated Timeline

- **Phase 1A (Rendering Fix):** 2-3 hours
- **Phase 1B (Verse Counts):** 1-2 hours
- **Phase 1C (Polish):** 1 hour
- **Testing & Debugging:** 1-2 hours

**Total:** 5-8 hours for complete Phase 1

---

## Next Steps After Phase 1

Once Phase 1 is complete:
1. **Phase 2 Planning:** Design SD card asset format
2. **Bible Text Source:** Find Douay-Rheims in structured format
3. **Asset Build Tool:** Create tool to convert text to binary format
4. **Storage Adapter:** Implement SD card file reading

But for now, **focus on Phase 1 only**.

---

## Notes

- Keep commits logical and atomic
- Test incrementally (don't wait until end)
- Document any deviations from plan
- Update roadmap.md as tasks complete

---

**Ready to begin?** This plan addresses all P0 and P1 blockers and completes Phase 1 of the roadmap.
