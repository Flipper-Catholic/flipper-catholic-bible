#!/usr/bin/env python3
"""Build rosary.bin from assets/source/rosary.json."""
import json
import os
import struct
import sys

MAGIC = 0x41534F52  # ROSA LE
VERSION = 1
MAX_STR = 2048


def w_str(f, s):
    b = (s or "").encode("utf-8")[:MAX_STR - 1]
    f.write(struct.pack("<H", len(b)))
    f.write(b)


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    src = os.path.join(root, "assets", "source", "rosary.json")
    out_dir = os.path.join(root, "files")
    if len(sys.argv) > 1:
        out_dir = sys.argv[1]
    with open(src, "r", encoding="utf-8") as f:
        data = json.load(f)
    os.makedirs(out_dir, exist_ok=True)
    path = os.path.join(out_dir, "rosary.bin")
    with open(path, "wb") as f:
        f.write(struct.pack("<I", MAGIC))
        f.write(struct.pack("<H", VERSION))
        w_str(f, data.get("how_to", ""))
        prayers = data.get("prayers", [])
        f.write(struct.pack("<H", len(prayers)))
        for p in prayers:
            w_str(f, p.get("title", ""))
            w_str(f, p.get("text", ""))
        mysteries = data.get("mysteries", {})
        for set_name in ["joyful", "sorrowful", "glorious", "luminous"]:
            arr = mysteries.get(set_name, [])
            f.write(struct.pack("<H", len(arr)))
            for m in arr:
                w_str(f, m.get("title", ""))
                w_str(f, m.get("meditation", ""))
                w_str(f, m.get("scripture", ""))
    print(f"Wrote rosary.bin: {len(prayers)} prayers, 4 mystery sets")


if __name__ == "__main__":
    main()
