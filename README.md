# Catholic Bible for Flipper Zero

> **Status:** Full Catholic Bible (73 books, 34,827 verses) is bundled in the FAP—install and read with no SD card required. For catalog submission: add app screenshots (qFlipper) and see [changelog](changelog.md). Details: [Status Report](STATUS_REPORT.md).

A fully offline Catholic Bible application for the Flipper Zero, featuring the complete Douay-Rheims Bible (full Catholic canon), verse-level navigation, bookmarks, history, and fast indexed search.

---

## ✝️ Overview

This app turns the Flipper Zero into a portable Scripture reader:

- 📖 Full Catholic Bible (Douay-Rheims, public domain) - ✅ **Bundled in FAP** (optional SD override)
- 🔍 Fast verse-level indexed search (offline) - *Planned (Phase 3)*
- 🧭 Book → Chapter → Verse navigation - ✅ **Implemented**
- 🔖 Bookmarks and recent history - ✅ **Implemented**
- 💾 microSD-based content and updates - ✅ **Infrastructure Ready**
- 📴 Fully offline, reverent, distraction-free - ✅ **Implemented**
- ✝️ Catholic Devotional Features - ✅ **UI Complete** (Missal, Rosary, Prayers, Confession)

The design intentionally avoids commentary, notes, audio, or online features to keep focus on the text itself.

---

## 📦 Current Features

### ✅ Implemented
- **Navigation Structure**: Full menu system (Menu → Books → Chapters → Verses)
- **Book Metadata**: All 73 Catholic canon books with chapter counts
- **Chapter Pagination**: Browse chapters with 40 chapters per page
- **Reader View**: ViewPort-based rendering, infinite scroll (Up/Down = prev/next verse), single-spaced full-width text
- **Full Bible**: 73 books, 34,827 verses bundled in FAP; optional SD `/apps_data/bible/` override
- **Verse Counts**: Accurate for Genesis (50 chapters); placeholder for other books
- **SD Card Storage**: Optional override; app works without SD
- **Bookmarks**: Full bookmark management; OK in reader toggles bookmark; Bookmarks menu lists all, tap to open verse
- **History**: Automatic history tracking and last-read verse; **Last read** menu item jumps to last verse; History menu lists recent verses, tap to open
- **Devotional Features**: UI structure for Missal, Rosary, Prayers, Confession

### 🚧 Optional / In Progress
- **Verse Counts**: Genesis complete; remaining 72 books use placeholder (Phase 1.3)
- **Catalog**: Add app screenshots (qFlipper) and submit manifest to Flipper Apps Catalog

### 📋 Planned Features

#### Reading & Navigation
- Full Catholic canon (73 books) - *Phase 2*
- Continuous reading with paging - *Phase 1*
- Structured picker navigation - ✅ **Done**
- Exact preservation of Scripture text - *Phase 2*

#### Search
- Indexed full-text search - *Phase 3*
- Partial-word matching (prefix-based) - *Phase 3*
- AND / OR search logic - *Phase 3*
- Highlighted search matches - *Phase 3*
- Canonical or relevance-based result ordering - *Phase 3*

#### Persistence
- Multiple bookmarks - ✅ **Implemented**
- Recent reading history - ✅ **Implemented**
- Automatic restore of last-read verse - ✅ **Implemented**

#### Reliability
- Guided recovery if assets are missing or corrupted - *Phase 5*
- Optional advanced reindexing - *Phase 5*
- Deterministic, auditable asset files - *Phase 2*

---

## 📁 Installation

### Easiest for users: one FAP with full Bible

- **Requirements:** Flipper Zero, microSD card, firmware that supports external apps.
- **Install:** Install the Catholic Bible FAP (from the Flipper app catalog or by building and sideloading). Put the microSD card in the Flipper.
- **First launch:** The app includes the full Douay-Rheims Bible (~35k verses). On first run, the system unpacks these assets to the SD card. No manual file copy is needed—just install the FAP and open the app.
- **Optional manual copy:** You can instead (or also) copy `bible_text.bin` and `verse_index.bin` to the SD card at `/apps_data/bible/`; the app prefers that path if present.

### Build from source

```bash
python3 -m pip install --upgrade ufbt   # if needed
ufbt build
# Install: copy the built .fap to the Flipper, or run:
ufbt launch
```

The repo includes pre-built assets in the `files/` folder so that `ufbt build` produces a FAP that contains the full Bible. The app appears under **Apps → Tools** with the cross icon.

**Reader:** Up/Down scroll; at edges, Up/Down = previous/next verse. Left/Right change verse. **OK** = toggle bookmark. Back exits to chapter list.  
**Menu:** **Last read**, **Bookmarks**, **History** as described above.

### Adding or updating Bible text manually

To use your own build of the assets (e.g. after editing `assets/source/bible_source.json`):

1. Run `python3 tools/build_bible_assets.py` (see **docs/full-bible-acquisition-plan.md** for the full Bible).
2. Copy `dist/apps_data/bible/bible_text.bin` and `verse_index.bin` to the Flipper SD at `/apps_data/bible/`.
3. To refresh the **bundled** copy (so the next FAP you build includes it), also copy those two files into the project’s `files/` folder and run `ufbt build` again. See `files/README.md`.

---

## 📚 Bible Text & Licensing

- Translation: Douay-Rheims
- License: Public Domain
- Canon: Full Catholic canon

---

## 🤝 Contributing

Contributions are welcome:
- Bug fixes
- Performance improvements
- Documentation
- Asset tooling improvements

Please keep the UI minimal and reverent, and follow the existing architecture and specs.

---

## 📊 Development Status

See [STATUS_REPORT.md](STATUS_REPORT.md) for detailed implementation status, known issues, and roadmap.

**Current Completion:** ~45-55% of planned features  
**Marketplace Readiness:** ⚠️ Beta/Early Access Ready (with disclaimers)  
**Testing Status:** ✅ Ready for device testing  
**Next Milestone:** Expand SD assets (full Genesis/Bible); Phase 3 search (index + engine); Phase 5 error handling

---

## 📜 License

- Code: MIT License (see [LICENSE](LICENSE))
- Bible Text (Douay-Rheims): Public Domain
