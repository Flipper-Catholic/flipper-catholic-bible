# Catholic Bible for Flipper Zero

> ⚠️ **Early Development Status**  
> This app is currently in active development. Core navigation works, but only Genesis 1:1-31 is available. Reader view has known issues. See [Status Report](STATUS_REPORT.md) for details.

A fully offline Catholic Bible application for the Flipper Zero, featuring the complete Douay-Rheims Bible (full Catholic canon), verse-level navigation, bookmarks, history, and fast indexed search.

---

## ✝️ Overview

This app turns the Flipper Zero into a portable Scripture reader:

- 📖 Full Catholic Bible (Douay-Rheims, public domain) - *Planned*
- 🔍 Fast verse-level indexed search (offline) - *Planned*
- 🧭 Book → Chapter → Verse navigation - ✅ **Implemented**
- 🔖 Bookmarks and recent history - *Planned*
- 💾 microSD-based content and updates - *Planned*
- 📴 Fully offline, reverent, distraction-free - ✅ **Designed**

The design intentionally avoids commentary, notes, audio, or online features to keep focus on the text itself.

---

## 📦 Current Features

### ✅ Implemented
- **Navigation Structure**: Full menu system (Menu → Books → Chapters → Verses)
- **Book Metadata**: All 73 Catholic canon books with chapter counts
- **Chapter Pagination**: Browse chapters with 40 chapters per page
- **Basic Reader View**: Scene structure and input handling (currently has rendering bug)
- **Sample Content**: Genesis 1:1-31 hardcoded for testing

### 🚧 In Progress
- **Reader View Rendering**: Fixing blank screen issue
- **Verse Count Data**: Replacing stub function with real verse counts

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
- Multiple bookmarks - *Phase 4*
- Recent reading history - *Phase 4*
- Automatic restore of last-read verse - *Phase 4*

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

**Current Completion:** ~15-20% of planned features  
**Marketplace Readiness:** ❌ Not Ready (see status report)  
**Next Milestone:** Fix reader view rendering + complete Phase 1.3 (verse counts)

---

## 📜 License

- Code: MIT License (see [LICENSE](LICENSE))
- Bible Text (Douay-Rheims): Public Domain
