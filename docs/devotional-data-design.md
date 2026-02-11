# Devotional Data Design (Missal, Rosary, Prayers, Confession)

Design notes for Phase 6 data: one asset file, sources, and year-dependent content.

---

## Goal: One data file for all devotional content

- **Single asset** (e.g. `devotional.bin` or `devotional.json` on SD / or bundled) for:
  - **Missal** – daily readings, prayers, responses; **year A/B/C** for Sunday lectionary.
  - **Rosary** – mysteries, prayers, optional scriptural verses.
  - **Common prayers** – index + full text (Sign of the Cross, Our Father, Hail Mary, etc.).
  - **Confession prep** – guide, examination of conscience, Acts of Contrition.
- Keeps install simple (one extra file or one FAP with Bible + devotional), fewer paths to break, and one format to version.

---

## Missal: year-to-year (A, B, C)

- **Sunday Lectionary** in the Roman Rite uses a **3-year cycle**: Year A (Matthew), B (Mark), C (Luke); John is used in specific seasons.
- **Weekday** readings use a **2-year cycle** (I and II).
- So Missal data is **date- and year-dependent**. Options:
  - **Precompute by calendar year** – e.g. “2026” bundle: all readings for 2026 (one A/B/C year + weekday cycle). One file per year or one file with a year index.
  - **On-device year** – app uses device date to pick “current” liturgical year (e.g. First Sunday of Advent 2025 → start of Year C), then looks up by (date, year_type).
- **Official US source:** [USCCB](https://www.usccb.org/) (United States Conference of Catholic Bishops) provides the [Daily Readings](https://www.usccb.org/) and [Lectionary](https://www.usccb.org/) content. Any data build pipeline should use USCCB (or an official derivative) for US users; ensure licensing/terms allow offline redistribution.
- **Single file layout:** e.g. header (magic, version, year_or_cycle), then sections: Missal (by date/key), Rosary, Prayers, Confession. Or one JSON with top-level keys `missal`, `rosary`, `prayers`, `confession` if we keep a text format for easier updates.

---

## Rosary, Prayers, Confession

- **Largely static** (same text across years). Can live in the same single file as sections.
- **Sources:** USCCB for prayers/readings; approved liturgical texts; catechism-based content for Confession prep (no device in the confessional).

---

## Asset format and path (chosen)

- **Path:** Same as Bible assets so one folder for the user: **`/apps_data/bible/devotional.json`** (and/or bundled in FAP under `files/devotional.json` with fallback like Bible).
- **Format:** JSON (easy to edit and extend; binary build step later if size demands).
- **Schema (v1):** `version`: 1; `missal`: object (readings by date/year A/B/C); `rosary`: object (mysteries, prayers); `prayers`: array of `{ "id", "title", "text" }`; `confession`: object (guide, examination, Acts of Contrition).
- **Source in repo:** `assets/source/devotional.json` – placeholder with empty structure; content added when Phase 6 is implemented.

---

## Suggested next steps (when implementing Phase 6)

1. ~~**Define one format**~~ – Done: single JSON, path above.
2. **Missal:** Populate missal with A/B/C and liturgical calendar; use device date to liturgical year then lookup. Source: USCCB.)
3. **Source pipeline:** Produce devotional.json from USCCB and approved texts; regenerate Missal section per liturgical year if needed.
4. **App:** Load devotional.json from same path as Bible assets (or bundled); parse and serve from this one file.

---

## References

- [USCCB – United States Conference of Catholic Bishops](https://www.usccb.org/) – Daily Readings, Lectionary, prayer/worship.
- Roman Missal (current typical form); Lectionary (3-year Sunday, 2-year weekday).
