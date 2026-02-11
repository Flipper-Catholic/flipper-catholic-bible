#!/usr/bin/env python3
"""
Build devotional.bin from assets/source/devotional.json (Phase 6).
Binary format: magic DEVO, version 1, num_prayers (uint16), then per prayer:
  title_len (uint16), title (UTF-8), text_len (uint16), text (UTF-8).
"""

import json
import os
import struct
import sys

MAGIC = 0x4F564544  # "DEVO" LE
VERSION = 1


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    src = os.path.join(root, "assets", "source", "devotional.json")
    out_dir = os.path.join(root, "files")
    if len(sys.argv) > 1:
        out_dir = sys.argv[1]
    with open(src, "r", encoding="utf-8") as f:
        data = json.load(f)
    prayers = data.get("prayers", [])
    os.makedirs(out_dir, exist_ok=True)
    path = os.path.join(out_dir, "devotional.bin")
    with open(path, "wb") as f:
        f.write(struct.pack("<I", MAGIC))
        f.write(struct.pack("<H", VERSION))
        f.write(struct.pack("<H", len(prayers)))
        for p in prayers:
            title = (p.get("title") or "").encode("utf-8")[:1023]
            text = (p.get("text") or "").encode("utf-8")[:8191]
            f.write(struct.pack("<H", len(title)))
            f.write(title)
            f.write(struct.pack("<H", len(text)))
            f.write(text)
    print(f"Wrote {len(prayers)} prayers to {path}")


if __name__ == "__main__":
    main()
