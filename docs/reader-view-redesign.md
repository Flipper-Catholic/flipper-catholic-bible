# Reader View Redesign: Fixing the Blank Screen

**Status:** ✅ Implemented  
**Goal:** Reliable reader with **up/down scrolling** within a verse and **infinite scroll** to move between verses (Up/Down at edge = previous/next verse).

---

## Why the Screen Is Blank

The current reader uses two drawing mechanisms that don’t match:

1. **What’s actually shown:** A **View** (`reader_view`) is added to the ViewDispatcher. When you switch to the reader, the dispatcher shows this View. The View has a draw callback, and the View has an **internal** ViewPort that gets attached to the GUI.

2. **What we tell to redraw:** We call `view_port_update(app->reader_viewport)`. That ViewPort is a **separate** object we allocate and never attach to the GUI. So we’re asking a viewport that isn’t on screen to redraw. The one that is on screen (inside the View) is never updated.

So either the first frame never gets a proper update, or we get one draw but no redraws on scroll/verse change. Either way, the visible result is a blank or stuck screen.

---

## Interaction Model (Implemented)

- **Up:** Scroll up within the verse; if already at top, go to **previous verse**.
- **Down:** Scroll down within the verse; if already at bottom, go to **next verse** (infinite scrolling).
- **Left / Right:** Also previous / next verse (alternate).
- **Back:** Exit reader (sends custom event; scene pops).

---

## Option 1: ViewPort-Only Reader (Recommended)

**Idea:** Use a **single ViewPort** for the reader. Don’t use a View for the reader at all.

- **Setup:** Allocate one ViewPort, set its draw and input callbacks (same logic as now: draw header + wrapped verse text with `scroll_offset`; input: Up/Down scroll, Left/Right prev/next verse).
- **When entering reader scene:**  
  `gui_add_view_port(app->gui, app->reader_viewport, GuiLayerFullscreen)`  
  (and set input callback so the viewport receives keys).
- **When exiting reader scene:**  
  `gui_remove_view_port(app->gui, app->reader_viewport)`.
- **Redraws:** Always call `view_port_update(app->reader_viewport)`. This is the viewport that’s actually on screen, so redraws work.

**Pros:**

- No View/ViewDispatcher mismatch; one owner, one viewport.
- Your existing draw and scroll/verse logic can stay; only the “who is shown” and “who is updated” change.
- Clear lifecycle: add on enter, remove on exit.

**Cons:**

- You manage the viewport and GUI layer yourself (add/remove). Input is handled on the viewport instead of through the View’s input callback.

**Code impact:** Remove the custom `reader_view` from the ViewDispatcher for the reader. Keep using the scene manager; in reader scene on_enter add the viewport to the GUI, in on_exit remove it. All `view_port_update` calls stay on `reader_viewport`. Optionally remove the unused View and the duplicate “orphan” viewport if you had one.

---

## Option 2: Fix the Current View-Based Approach

**Idea:** Keep using the View for the reader but trigger redraws on the **view’s** viewport, not the orphan one.

- In the Flipper SDK, the View has an internal ViewPort. Some apps use `view_get_view_port(view)` (internal API) or rely on `view_set_update_callback`. The update callback is “called upon model change, need to be propagated to GUI through ViewPort update.”
- So either:
  - Get the view’s viewport (if the app has access to something like `view_get_view_port`) and call `view_port_update` on that, or
  - Set `view_set_update_callback` and, inside that callback, trigger the view’s viewport update (implementation depends on what the SDK exposes to apps).

**Pros:** Stays within View + ViewDispatcher.

**Cons:** Depends on internal or less-documented API; still two concepts (View + ViewPort) for one screen.

---

## Option 3: Use the TextBox Module for the Reader

**Idea:** Don’t draw the verse yourself. Build a string “Genesis 1:1\n\nIn the beginning…”, put it in the TextBox, and show the TextBox view when in the reader scene. TextBox has built-in scrolling. Use a custom input callback on the TextBox view to intercept Left/Right (and optionally Back/OK) for prev/next verse; let Up/Down go to the TextBox for scrolling.

**Pros:** Minimal custom drawing; TextBox is maintained by the SDK and usually works.

**Cons:** Less control over layout (e.g. fixed header line); need to confirm TextBox scroll behavior and line length on device. You already noted “TextBox scrolling with Up/Down may not work properly” — so this is a fallback if Option 1 or 2 is harder.

---

## Recommendation

**Prefer Option 1 (ViewPort-only reader):**

1. Fixes the blank screen by making the only viewport we update the one that’s actually on screen.
2. Keeps your desired interaction: up/down = scroll verse, left/right = prev/next verse.
3. Reuses your existing draw and input logic; only attachment and lifecycle change.
4. Avoids depending on View internals or TextBox quirks.

**Suggested steps when you implement:**

1. In reader scene **on_enter:**  
   - Load verse text and set `scroll_offset = 0` (as now).  
   - Add the reader ViewPort to the GUI (e.g. `gui_add_view_port(..., GuiLayerFullscreen)`).  
   - Call `view_port_update(app->reader_viewport)` once so the first frame draws.
2. In reader scene **on_exit:**  
   - Remove the reader ViewPort from the GUI (`gui_remove_view_port`).
3. In the reader ViewPort **input callback:**  
   - Up/Down: adjust `scroll_offset`, then `view_port_update(app->reader_viewport)`.  
   - Left/Right: send custom events for prev/next verse (scene manager reloads reader with new verse).  
   - Back: let it close the reader (or forward to view dispatcher back).
4. Remove the custom View from the ViewDispatcher for the reader (and any duplicate/orphan viewport), so there’s a single, clear path: one ViewPort, add on enter, remove on exit, update when scroll or verse changes.

If you want, next step can be a concrete patch (edits to `catholic_bible.c`) following Option 1 exactly.
