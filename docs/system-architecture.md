# System Architecture Overview
Catholic Bible App for Flipper Zero

## Architectural Goals
- Fully offline
- Tight RAM discipline
- Verse-level navigation + indexed search
- Code/content separation (SD assets)
- Simple, auditable, deterministic behavior

## High-Level Architecture

Flipper App
- UI Layer
  - Reader View
  - Book/Chapter/Verse Picker
  - Search UI + Results
  - Bookmarks + History UI
  - Recovery UI
- App Logic Layer
  - Navigation Engine
  - Search Engine
  - Bookmark Manager
  - History Manager
- Storage Adapter
  - SD file access
  - Integrity checks
  - Compatibility checks
  - Recovery helpers

microSD (/apps_data/bible/)
- bible_text.bin
- verse_index.bin
- canon_table.bin
- shard_map.bin
- shards/shard_*.bin
- metadata.json

## Core Components

### UI Layer
- Renders text and menus
- Handles input
- No file access
- No indexing logic

### Navigation Engine
- Resolves Book/Chapter/Verse to VerseID
- VerseID to text offsets
- Drives continuous paging across verses

### Search Engine
- Loads only required index shards
- Partial-word matching (prefix)
- AND/OR logic
- Returns VerseIDs

### Bookmark & History Managers
- Store VerseIDs
- Persist to Flipper app storage (not SD assets)

### Storage Adapter
- The only component that touches SD files
- Ensures bounded memory usage and safe failures

## Failure & Recovery Model
- Missing SD or missing/corrupt assets triggers guided recovery mode
- If text assets load but search assets missing: reading remains available, search disabled with explanation

## Security & Trust
- No network usage
- No external execution
- Deterministic rendering and file formats
- Auditable assets on SD

## Forward Dependencies
This document supports:
- Data & Index File Layout Specification
- Backlog stories for storage + search
- Developer installation and tooling docs
