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
- **Reader View**: ViewPort-based rendering with scrolling and navigation
- **Sample Content**: Genesis 1:1-31 hardcoded for testing
- **Verse Counts**: Accurate counts for Genesis (all 50 chapters)
- **SD Card Storage**: Infrastructure complete (ready for asset files)
- **Bookmarks**: Full bookmark management with persistent storage
- **History**: Automatic history tracking and last-read verse
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

> ⚠️ **Note**: Currently only Genesis 1:1-31 is available. Full Bible text loading via SD card is planned for Phase 2.

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

**Current Completion:** ~40-50% of planned features  
**Marketplace Readiness:** ⚠️ Beta/Early Access Ready (with disclaimers)  
**Testing Status:** ✅ Ready for device testing  
**Next Milestone:** Device testing + Phase 2.1 (asset build tool for full Bible)

---

## 📜 License

- Code: MIT License (see [LICENSE](LICENSE))
- Bible Text (Douay-Rheims): Public Domain
