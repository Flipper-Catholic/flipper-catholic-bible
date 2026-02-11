# Phase 6 Completion & Phase 7 Plan

Execution order: Phase 6 (Missal, Rosary, Confession) → Polish → Search (AND/OR, ranking, pagination) → Phase 7 (Guides + indices) → **Testing (2nd-to-last)** → Catalog/screenshots.

---

## Phase 6 Completion

### 6.1 Missal (complete)
- **Data:** Liturgical calendar (seasons: Advent, Christmas, Lent, Easter, Ordinary Time; year A/B/C). Daily readings keyed by liturgical key (e.g. year_type + season + week + day). Mass prayers & responses (static).
- **Format:** Extended `devotional.json` → build outputs `devotional.bin` (prayers) + `missal.bin` (calendar metadata + readings index + text blobs). Or single extended binary with section headers.
- **UI:** Today's Mass (device date → liturgical key → show readings + prayers); Browse by Date; Liturgical Calendar (list seasons); Mass Prayers; Mass Responses; Readings Search (reuse Bible search or simple keyword).

### 6.2 Rosary (complete)
- **Data:** 20 mysteries (4 sets of 5), each with title, short meditation, optional scripture ref. Rosary prayers (Our Father, Hail Mary, Glory Be, Fatima, Hail Holy Queen). How-to guide.
- **Format:** `rosary.bin` or section in extended devotional asset.
- **UI:** How to Pray; Joyful/Sorrowful/Glorious/Luminous (submenu → mystery list → meditation + optional verse); Rosary Prayers (text list); Scriptural Rosary; Intentions.

### 6.4 Confession (complete)
- **Data:** Guide (steps, what to say); Examination (10 Commandments, 7 Deadly Sins, Q&A); Acts of Contrition (multiple forms); tips; post-Confession.
- **Format:** `confession.bin` or section in extended devotional.
- **UI:** Confession Guide; 10 Commandments; 7 Deadly Sins; Examination Q&A; Act of Contrition; Confession Prayers; Tips; Post-Confession.

---

## Phase 7 (Guides & Indices)

### Priority: "Guides" top-level menu
- Add **Guides** as a main menu item (alongside Browse, Last read, Search, Missal, Rosary, Prayers, Confession, Bookmarks, History, About).
- Submenu: Order of Mass, OCIA Guide, Lenten Guide, Easter Guide, Pentecost Guide, **Sacraments Guide**, **Marrying Catholic Guide** (and later: Saints, KoC, Church Locations, Holy Orders, Vatican, Holy Sites as needed).

### Priority: Sacraments of the Catholic Church (Guide)
- **Content:** Basic guide on the seven Sacraments of the Church. **Structure:** (1) Overview – what a sacrament is (outward sign, instituted by Christ, gives grace). (2) The seven: Baptism, Confirmation, Eucharist, Penance/Reconciliation, Anointing of the Sick, Holy Orders, Matrimony. For each: brief definition, who administers, who receives, effects, and when/where commonly celebrated. (3) Sacraments of Initiation (Baptism, Confirmation, Eucharist) as a group. (4) Sacraments of Healing (Penance, Anointing). (5) Sacraments at the Service of Communion (Orders, Matrimony). Not year-dependent.
- **Data/Format:** Optional `guides/sacraments.json` → `sacraments.bin` (sections + text blobs) or single scrollable guide; same pattern as other guides (loader + scene). Scaffold can use in-app placeholder text until .bin exists.
- **Scaffolding:** Full: plan above; app submenu item "Sacraments Guide"; scene that shows scrollable guide (placeholder or loaded). Sections can be submenu (Overview, each sacrament, groupings) or one long scroll.

### Priority: Marrying Catholic (Guide)
- **Content:** Basic guide on Marriage in the Church and preparation. **Audiences:** (1) Two Catholics marrying. (2) Catholic + other Christian (mixed marriage). (3) Catholic + non-Christian (disparity of cult). **Structure:** (1) Overview – marriage as sacrament; validity (consent, form, no impediments). (2) Preparation – FOCCUS/Pre-Cana, meetings with priest/deacon, documents (baptismal certificate, dispensations if applicable), timeline. (3) Catholic–Catholic: Nuptial Mass, readings, vows, typical timeline. (4) Catholic–other Christian: dispensation, form of celebration (Mass vs. outside Mass), pastoral guidance. (5) Catholic–non-Christian: disparity of cult dispensation, promises (raising children Catholic), form. (6) Common questions: convalidation, annulments (mention only; direct to pastor). Not year-dependent.
- **Data/Format:** Optional `guides/marrying_catholic.json` → `marrying_catholic.bin`; sections for Overview, Preparation, Catholic–Catholic, Mixed, Disparity, FAQ. Scaffold can use in-app placeholder text.
- **Scaffolding:** Full: plan above; app submenu item "Marrying Catholic Guide"; scene with scrollable text or submenu by section (Overview, Preparation, Catholic–Catholic, Catholic–Other Christian, Catholic–Non-Christian, FAQ).

### Priority: Order of Mass (Guide)
- **Content:** Basic guide to the common Orders of Mass (Ordinary Form). Not year-dependent. Structure: Introductory Rites, Liturgy of the Word, Liturgy of the Eucharist, Concluding Rites. Brief descriptions + key texts (Lord have mercy, Gloria, Creed, Holy Holy, etc.).
- **Scaffolding:** Full: data file (e.g. `guides/order_of_mass.bin` or JSON), loader, scene(s) with scrollable text / submenu by part.

### Priority: OCIA Guide
- **Content:** Basic guide on OCIA (Order of Christian Initiation of Adults) and entrance process. **Two tracks:** (1) Someone coming into the Church; (2) Sponsor.
- **Scaffolding:** Full: data, loader, scenes with submenu for Inquirer vs Sponsor, then sections (stages, rites, what to expect, etc.).

### Priority: Lenten Guide
- **Content:** Lenten practices (prayer, fasting, almsgiving), key dates (Ash Wednesday, Holy Week), Stations of the Cross summary. Not year-specific.
- **Scaffolding:** Full.

### Priority: Easter Guide
- **Content:** Easter season practices, Pentecost, Octave of Easter. Not year-specific.
- **Scaffolding:** Full.

### Priority: Pentecost Guide
- **Content:** Pentecost meaning and practices. Not year-specific.
- **Scaffolding:** Full.

### Index of Saints
- **Content:** Name; status & type of canonization; feast/liturgical day (calendar-based, not actual year); important dates (birth, death, etc.); up to ~10 sentence paragraph (placeholder OK for scaffold).
- **UI:** Scrolling view, saint name at top (like Verses). Prioritize technical scaffolding and data structure; then UI.
- **Data sources:** Research (Butler's, Vatican, etc.); scaffold with a few example saints.

### Knights of Columbus
- **Content:** General info; Fraternal/Family & Financial Benefits; History; Fr. McGivney's story & beatification status; Council index (Name, City, State, Number) with search.
- **Scaffolding:** Technical first; UI scaffold; data sources research.

### Church Locations Index
- **Explore only:** Minor UI/UX and technical scaffolding as planning. Idea: Roman Catholic churches in US (Name, Street, City, State, Zip, Website); search. **Submenus:** (1) Eastern Catholic Churches (23) that Roman Catholics may fulfill obligation / Communion when traveling (with note on special rules); (2) Emergency: Greek Orthodox, Russian Orthodox + canon law note (844: danger of death/grave necessity, valid sacraments, etc.); (3) Restricted: ELCA, Episcopal, UMC, Baptist – no Communion, apostolic succession note.

### Holy Orders Index
- **Scaffolding only:** Index of Name, Mission, Brief history. Add a few example orders in scaffold.

### Vatican Information
- **Scaffolding only:** Offices, Pope, top cardinals – basic info.

### Holy Sites & Locations Index
- **Scaffolding only:** World holy sites index.

---

## Testing (2nd-to-last)
- One massive test pass: all menus, Bible read/search, Prayers, Missal, Rosary, Confession, Guides (Order of Mass, OCIA, Lent, Easter, Pentecost, Sacraments, Marrying Catholic), Saints/KoC/Church/Orders/Vatican/Sites scaffolds, bookmarks, history, back navigation, no crashes.

---

## Catalog (last)
- Screenshots (qFlipper); manifest.yml PR.
