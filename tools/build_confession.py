#!/usr/bin/env python3
"""Build confession.bin from assets/source/confession.json."""
import json
import os
import struct
import sys

MAGIC = 0x464E4F43  # CONF LE
VERSION = 1
MAX_STR = 2048


def w_str(f, s):
    b = (s or "").encode("utf-8")[:MAX_STR - 1]
    f.write(struct.pack("<H", len(b)))
    f.write(b)


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    src = os.path.join(root, "assets", "source", "confession.json")
    out_dir = os.path.join(root, "files")
    if len(sys.argv) > 1:
        out_dir = sys.argv[1]
    with open(src, "r", encoding="utf-8") as f:
        data = json.load(f)
    os.makedirs(out_dir, exist_ok=True)
    path = os.path.join(out_dir, "confession.bin")
    with open(path, "wb") as f:
        f.write(struct.pack("<I", MAGIC))
        f.write(struct.pack("<H", VERSION))
        w_str(f, data.get("guide", ""))
        cmdts = data.get("ten_commandments", [])
        f.write(struct.pack("<H", len(cmdts)))
        for c in cmdts:
            w_str(f, c)
        sins = data.get("deadly_sins", [])
        f.write(struct.pack("<H", len(sins)))
        for s in sins:
            w_str(f, s.get("name", ""))
            w_str(f, s.get("description", ""))
        exam = data.get("examination", [])
        f.write(struct.pack("<H", len(exam)))
        for e in exam:
            w_str(f, e.get("category", ""))
            w_str(f, e.get("question", ""))
        acts = data.get("acts_of_contrition", [])
        f.write(struct.pack("<H", len(acts)))
        for a in acts:
            w_str(f, a.get("title", ""))
            w_str(f, a.get("text", ""))
        w_str(f, data.get("tips", ""))
        w_str(f, data.get("post", ""))
    print(f"Wrote confession.bin: {len(cmdts)} cmdts, {len(sins)} sins, {len(exam)} exam, {len(acts)} acts")


if __name__ == "__main__":
    main()
