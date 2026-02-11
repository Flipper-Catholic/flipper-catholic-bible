# Catholic Bible for Flipper Zero

> ⚠️ **Development Status**  
> Core features are implemented and ready for testing. Navigation, reading (Genesis 1:1-31), bookmarks, and history are functional. Full Bible content requires SD card assets. See [Status Report](STATUS_REPORT.md) for details.

A fully offline Catholic Bible application for the Flipper Zero, featuring the complete Douay-Rheims Bible (full Catholic canon), verse-level navigation, bookmarks, history, and fast indexed search.

---

## ✝️ Overview

This app turns the Flipper Zero into a portable Scripture reader:

- 📖 Full Catholic Bible (Douay-Rheims, public domain) - *Infrastructure Ready*
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
- **Sample Content**: Genesis 1:1-31 in-app; Genesis 1–2 (56 verses) via SD assets (see Adding Bible text)
- **Verse Counts**: Accurate counts for Genesis (all 50 chapters)
- **SD Card Storage**: Infrastructure complete (ready for asset files)
- **Bookmarks**: Full bookmark management; OK in reader toggles bookmark; Bookmarks menu lists all, tap to open verse
- **History**: Automatic history tracking and last-read verse; **Last read** menu item jumps to last verse; History menu lists recent verses, tap to open
- **Devotional Features**: UI structure for Missal, Rosary, Prayers, Confession

### 🚧 In Progress
- **Full Bible Content**: Only Genesis 1:1-31 available (infrastructure ready for full Bible)
- **Verse Counts**: Genesis complete, other books use placeholder

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

### Requirements
- Flipper Zero
- microSD card (required)
- Flipper firmware supporting external apps

### Install from Source (Current)
This app is in early development. To build and install:

```bash
# Install ufbt if not already installed
python3 -m pip install --upgrade ufbt

# Build and launch
ufbt launch
```

When installed as a FAP, the app appears under **Apps → Tools** and uses the custom cross icon.  
**Reader:** Up/Down scroll; at edges, Up/Down = previous/next verse. Left/Right change verse. **OK** = toggle bookmark (header shows "(BM)" when bookmarked). Back exits reader or returns from lists.  
**Menu:** **Last read** opens the last-read verse (or History if none). **Bookmarks** and **History** show lists; select a verse to open it in the reader.

> ⚠️ **Note**: Without SD assets, only Genesis 1:1-31 is available in-app. To add more verses (e.g. Genesis 1–2, or full Bible), see **Adding Bible text** below.

### Adding Bible text (SD card)

To load more verses from the SD card:

1. Build assets from the included source (Genesis 1–2):
   ```bash
   python3 tools/build_bible_assets.py
   ```
2. Copy the contents of `dist/apps_data/bible/` to your Flipper SD card at `/apps_data/bible/` (create the folder if needed).
3. The app will use `bible_text.bin` and `verse_index.bin` when present. To add all verses for more books, edit `assets/source/bible_source.json` and re-run the script. See `tools/README.md` for details.

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
