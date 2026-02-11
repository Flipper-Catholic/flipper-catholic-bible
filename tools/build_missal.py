#!/usr/bin/env python3
"""Build missal.bin from assets/source/missal.json. Format: magic MISS, version, seasons, mass_prayers, mass_responses, readings."""
import json
import os
import struct
import sys

MAGIC = 0x5353494D  # MISS LE
VERSION = 1
MAX_STR = 2048


def w_str(f, s):
    b = (s or "").encode("utf-8")[:MAX_STR - 1]
    f.write(struct.pack("<H", len(b)))
    f.write(b)


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    src = os.path.join(root, "assets", "source", "missal.json")
    out_dir = os.path.join(root, "files")
    if len(sys.argv) > 1:
        out_dir = sys.argv[1]
    with open(src, "r", encoding="utf-8") as f:
        data = json.load(f)
    os.makedirs(out_dir, exist_ok=True)
    path = os.path.join(out_dir, "missal.bin")
    with open(path, "wb") as f:
        f.write(struct.pack("<I", MAGIC))
        f.write(struct.pack("<H", VERSION))
        seasons = data.get("seasons", [])
        f.write(struct.pack("<H", len(seasons)))
        for s in seasons:
            w_str(f, s.get("name", ""))
            w_str(f, s.get("description", ""))
        prayers = data.get("mass_prayers", [])
        f.write(struct.pack("<H", len(prayers)))
        for p in prayers:
            w_str(f, p.get("title", ""))
            w_str(f, p.get("text", ""))
        responses = data.get("mass_responses", [])
        f.write(struct.pack("<H", len(responses)))
        for r in responses:
            w_str(f, r.get("title", ""))
            w_str(f, r.get("text", ""))
        readings = data.get("readings", {})
        keys = sorted(readings.keys())
        f.write(struct.pack("<H", len(keys)))
        for k in keys:
            r = readings[k]
            key_b = k.encode("utf-8")[:64]
            f.write(struct.pack("<B", len(key_b)))
            f.write(key_b)
            w_str(f, r.get("title", ""))
            w_str(f, r.get("first_reading", ""))
            w_str(f, r.get("psalm", ""))
            w_str(f, r.get("gospel", ""))
    print(f"Wrote missal.bin: {len(seasons)} seasons, {len(prayers)} prayers, {len(responses)} responses, {len(keys)} readings")


if __name__ == "__main__":
    main()
