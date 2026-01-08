#!/usr/bin/env python3
from __future__ import annotations

import re
from pathlib import Path

SRC = Path("src/catholic_bible.c")

CALLBACK_NAME = "catholic_bible_reader_textbox_input_callback"
TEXTBOX_VIEW_ID = "CatholicBibleViewTextBox"


def ensure_reader_input_callback(code: str) -> str:
    # If callback already exists, do nothing.
    if re.search(rf"\bstatic\s+bool\s+{re.escape(CALLBACK_NAME)}\s*\(", code):
        return code

    # Insert callback near other helper funcs (before App lifecycle is fine).
    insert_point = re.search(r"/\*\s*={20,}\s*\n\s*\*\s*App lifecycle", code)
    if not insert_point:
        raise RuntimeError("Could not find 'App lifecycle' section to insert callback.")

    callback = f"""
static bool {CALLBACK_NAME}(InputEvent* event, void* context) {{
    CatholicBibleApp* app = context;

    // Only intercept SHORT presses
    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyLeft) {{
        view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_PREV_VERSE);
        return true; // consume Left
    }}

    if(event->key == InputKeyRight) {{
        view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_NEXT_VERSE);
        return true; // consume Right
    }}

    // IMPORTANT: do NOT consume Up/Down; TextBox needs them for scrolling
    return false;
}}

"""
    return code[: insert_point.start()] + callback + code[insert_point.start() :]


def remove_duplicate_textbox_add_view(code: str) -> str:
    """
    Remove duplicate view_dispatcher_add_view(... CatholicBibleViewTextBox ...)
    keeping exactly one (preferably the one in the "Register views" block).
    """
    lines = code.splitlines(True)
    add_view_idxs = [
        i for i, ln in enumerate(lines)
        if "view_dispatcher_add_view" in ln and TEXTBOX_VIEW_ID in ln and "text_box_get_view" in ln
    ]
    if len(add_view_idxs) <= 1:
        return code

    # Prefer keeping the one that occurs after "Register views" comment
    keep_idx = None
    for idx in add_view_idxs:
        window = "".join(lines[max(0, idx-8):idx+1])
        if "Register views" in window or "Register view" in window:
            keep_idx = idx
            break

    if keep_idx is None:
        # Otherwise keep the last occurrence (usually in the proper block)
        keep_idx = add_view_idxs[-1]

    # Remove all others
    for idx in reversed(add_view_idxs):
        if idx != keep_idx:
            lines.pop(idx)

    return "".join(lines)


def ensure_textbox_view_input_hook(code: str) -> str:
    """
    Ensure we call view_set_context and view_set_input_callback on the TextBox view.
    We attach these immediately AFTER the (single) add_view for TextBox.
    """
    # If already hooked with our callback name, do nothing.
    if CALLBACK_NAME in code and "view_set_input_callback" in code and TEXTBOX_VIEW_ID in code:
        # Still might have no actual call; check explicitly for callback usage
        if re.search(rf"view_set_input_callback\s*\(\s*text_box_get_view\([^)]+\)\s*,\s*{re.escape(CALLBACK_NAME)}\s*\)", code):
            return code
        if re.search(rf"view_set_input_callback\s*\(\s*\w+\s*,\s*{re.escape(CALLBACK_NAME)}\s*\)", code):
            return code

    pattern = re.compile(
        r"(view_dispatcher_add_view\(\s*app->view_dispatcher\s*,\s*"
        + re.escape(TEXTBOX_VIEW_ID)
        + r"\s*,\s*text_box_get_view\(\s*app->text_box\s*\)\s*\)\s*;\s*)"
    )

    m = pattern.search(code)
    if not m:
        raise RuntimeError("Could not find the TextBox view registration line to hook input callback.")

    hook = (
        m.group(1)
        + "\n    // Hybrid reader controls: Left/Right = verse change, Up/Down = native TextBox scroll\n"
        + "    View* tb_view = text_box_get_view(app->text_box);\n"
        + "    view_set_context(tb_view, app);\n"
        + f"    view_set_input_callback(tb_view, {CALLBACK_NAME});\n"
    )

    # Replace only the first match (there should be exactly one after duplicate removal)
    return code[: m.start()] + hook + code[m.end() :]


def main() -> None:
    if not SRC.exists():
        raise SystemExit(f"ERROR: {SRC} not found. Run this from the repo root.")

    original = SRC.read_text(encoding="utf-8")
    backup = SRC.with_suffix(".c.bak")
    backup.write_text(original, encoding="utf-8")

    code = original
    code = remove_duplicate_textbox_add_view(code)
    code = ensure_reader_input_callback(code)
    code = ensure_textbox_view_input_hook(code)

    if code == original:
        print("No changes needed (already patched).")
        return

    SRC.write_text(code, encoding="utf-8")
    print(f"Patched {SRC} (backup saved to {backup})")


if __name__ == "__main__":
    main()
