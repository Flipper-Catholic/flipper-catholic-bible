#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/view.h>
#include <gui/view_port.h>
#include <gui/canvas.h>

#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_box.h>

#include <furi_hal_resources.h>
#include <string.h>

#include "books_meta.h"
#include "storage_adapter.h"
#include "bookmark_manager.h"
#include "history_manager.h"

#define CHAPTERS_PER_PAGE 40

#define CH_EVT_PREV_PAGE     0xF0000001u
#define CH_EVT_NEXT_PAGE     0xF0000002u

#define CH_EVT_CHAPTER_BASE  0x00010000u
#define VS_EVT_VERSE_BASE    0x00020000u

#define CH_ENCODE_CHAPTER(c) (CH_EVT_CHAPTER_BASE | ((uint32_t)(c) & 0xFFFFu))
#define CH_IS_CHAPTER(e)     (((e) & 0xFFFF0000u) == CH_EVT_CHAPTER_BASE)
#define CH_DECODE_CHAPTER(e) ((uint16_t)((e) & 0xFFFFu))

#define VS_ENCODE_VERSE(v)   (VS_EVT_VERSE_BASE | ((uint32_t)(v) & 0xFFFFu))
#define VS_IS_VERSE(e)       (((e) & 0xFFFF0000u) == VS_EVT_VERSE_BASE)
#define VS_DECODE_VERSE(e)   ((uint16_t)((e) & 0xFFFFu))

#define READER_EVT_PREV_VERSE  0x91000001u
#define READER_EVT_NEXT_VERSE  0x91000002u
#define READER_EVT_BACK        0x91000003u
#define READER_EVT_TOGGLE_BM   0x91000004u

/* Forward declaration - use struct keyword for incomplete type */
struct CatholicBibleApp;

/* ============================================================================
 * books_meta.h helpers (expects your now-working “real” books_meta.h)
 * ==========================================================================*/

static size_t cb_books_count(void) {
#ifdef CATHOLIC_BIBLE_BOOKS_COUNT
    return (size_t)CATHOLIC_BIBLE_BOOKS_COUNT;
#elif defined(BOOKS_COUNT)
    return (size_t)BOOKS_COUNT;
#elif defined(BOOKS_META_COUNT)
    return (size_t)BOOKS_META_COUNT;
#elif defined(books_count)
    return (size_t)books_count;
#else
    return 1;
#endif
}

static const char* cb_book_name(size_t i) {
#ifdef CATHOLIC_BIBLE_BOOKS_COUNT
    return catholic_bible_book_names[i];
#elif defined(BOOKS_COUNT)
    extern const char* const book_names[];
    return book_names[i];
#elif defined(books_count)
    extern const char* const books_names[];
    return books_names[i];
#else
    (void)i;
    return "Genesis (stub)";
#endif
}

static uint16_t cb_book_chapters(size_t i) {
#ifdef CATHOLIC_BIBLE_BOOKS_COUNT
    return catholic_bible_book_chapter_counts[i];
#elif defined(BOOKS_COUNT)
    extern const uint16_t book_chapter_counts[];
    return book_chapter_counts[i];
#elif defined(books_count)
    extern const uint16_t books_chapter_counts[];
    return books_chapter_counts[i];
#else
    (void)i;
    return 50;
#endif
}

/* ============================================================================
 * Sample Verse Data (Genesis 1:1-31) - Douay-Rheims
 * Phase 1.2: Hardcoded sample verses for proof-of-concept
 * Phase 2: Will be replaced with SD card-based asset loading
 * ==========================================================================*/

#define GENESIS_1_VERSE_COUNT 31

static const char* const genesis1_verses[GENESIS_1_VERSE_COUNT] __attribute__((used)) = {
    "In the beginning God created heaven, and earth.",
    "And the earth was void and empty, and darkness was upon the face of the deep; and the spirit of God moved over the waters.",
    "And God said: Be light made. And light was made.",
    "And God saw the light that it was good; and he divided the light from the darkness.",
    "And he called the light Day, and the darkness Night; and there was evening and morning one day.",
    "And God said: Let there be a firmament made amidst the waters: and let it divide the waters from the waters.",
    "And God made a firmament, and divided the waters that were under the firmament, from those that were above the firmament, and it was so.",
    "And God called the firmament, Heaven; and the evening and morning were the second day.",
    "God also said: Let the waters that are under the heaven, be gathered together into one place: and let the dry land appear. And it was so done.",
    "And God called the dry land, Earth; and the gathering together of the waters, he called Seas. And God saw that it was good.",
    "And he said: Let the earth bring forth the green herb, and such as may seed, and the fruit tree yielding fruit after its kind, which may have seed in itself upon the earth. And it was so done.",
    "And the earth brought forth the green herb, and such as yieldeth seed according to its kind, and the tree that beareth fruit, having seed each one according to its kind. And God saw that it was good.",
    "And the evening and the morning were the third day.",
    "And God said: Let there be lights made in the firmament of heaven, to divide the day and the night, and let them be for signs, and for seasons, and for days and years:",
    "To shine in the firmament of heaven, and to give light upon the earth. And it was so done.",
    "And God made two great lights: a greater light to rule the day; and a lesser light to rule the night: and the stars.",
    "And he set them in the firmament of heaven to shine upon the earth.",
    "And to rule the day and the night, and to divide the light and the darkness. And God saw that it was good.",
    "And the evening and morning were the fourth day.",
    "God also said: Let the waters bring forth the creeping creature having life, and the fowl that may fly over the earth under the firmament of heaven.",
    "And God created the great whales, and every living and moving creature, which the waters brought forth, according to their kinds, and every winged fowl according to its kind. And God saw that it was good.",
    "And he blessed them, saying: Increase and multiply, and fill the waters of the sea: and let the birds be multiplied upon the earth.",
    "And the evening and morning were the fifth day.",
    "And God said: Let the earth bring forth the living creature in its kind, cattle and creeping things, and beasts of the earth, according to their kinds. And it was so done.",
    "And God made the beasts of the earth according to their kinds, and cattle, and every thing that creepeth on the earth after its kind. And God saw that it was good.",
    "And he said: Let us make man to our image and likeness: and let him have dominion over the fishes of the sea, and the fowls of the air, and the beasts, and the whole earth, and every creeping creature that moveth upon the earth.",
    "And God created man to his own image: to the image of God he created him: male and female he created them.",
    "And God blessed them, saying: Increase and multiply, and fill the earth, and subdue it, and rule over the fishes of the sea, and the fowls of the air, and all living creatures that move upon the earth.",
    "And God said: Behold I have given you every herb bearing seed upon the earth, and all trees that have in themselves seed of their own kind, to be your meat:",
    "And to all beasts of the earth, and to every fowl of the air, and to all that move upon the earth, and wherein there is life, that they may have to feed upon. And it was so done.",
    "And God saw all the things that he had made, and they were very good. And the evening and morning were the sixth day."
};


/* ============================================================================
 * App definitions
 * ==========================================================================*/

typedef enum {
    CatholicBibleViewSubmenu = 0,
    CatholicBibleViewWidget,
    CatholicBibleViewTextBox,
    CatholicBibleViewReader,
} CatholicBibleViewId;

typedef enum {
    CatholicBibleSceneMenu = 0,
    CatholicBibleSceneBrowseBooks,
    CatholicBibleSceneBrowseChapters,
    CatholicBibleSceneBrowseVerses,
    CatholicBibleSceneReader,
    CatholicBibleSceneSearch,
    CatholicBibleSceneMissal,
    CatholicBibleSceneRosary,
    CatholicBibleScenePrayers,
    CatholicBibleSceneConfession,
    CatholicBibleSceneBookmarks,
    CatholicBibleSceneHistory,
    CatholicBibleSceneAbout,
    CatholicBibleSceneCount,
} CatholicBibleSceneId;

typedef enum {
    MenuItemBrowse = 0,
    MenuItemLastRead,
    MenuItemSearch,
    MenuItemMissal,
    MenuItemRosary,
    MenuItemPrayers,
    MenuItemConfession,
    MenuItemBookmarks,
    MenuItemHistory,
    MenuItemAbout,
} MenuItemId;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Submenu* submenu;
    Widget* widget;
    TextBox* text_box;
    ViewPort* reader_viewport; // Reader: added to GUI in reader scene only

    // Browse state
    size_t selected_book_index;
    uint16_t selected_chapter; // 1-based
    uint16_t selected_verse;   // 1-based
    uint16_t chapter_page;
    
    // Reader view state
    int32_t scroll_offset;       // Scroll position (pixels)
    int32_t reader_content_height; // Total content height (pixels), set by draw callback
    const char* current_verse_text; // Current verse text being displayed
    char* current_verse_buffer; // Buffer for SD card-loaded verse text
    
    // Storage adapter (Phase 2.2)
    StorageAdapter storage;
    
    // Bookmark and history managers (Phase 4)
    BookmarkManager bookmarks;
    HistoryManager history;
} CatholicBibleApp;

/* Verse count lookup - Phase 2.3: Uses storage adapter with fallback to hardcoded */
static uint16_t cb_chapter_verses(CatholicBibleApp* app, size_t book_index, uint16_t chapter_1based) {
    if(!app) return 50; // Default fallback
    
    // Try storage adapter first (Phase 2.2)
    if(storage_adapter_assets_available(&app->storage)) {
        uint16_t count = storage_adapter_get_verse_count(&app->storage, book_index, chapter_1based);
        if(count > 0) {
            return count;
        }
        // Fall through to hardcoded if storage fails
    }
    
    // Fallback to hardcoded data (Phase 1.3)
    // Bounds checking
    if(book_index >= CATHOLIC_BIBLE_BOOKS_COUNT) return 50; // Default fallback
    if(chapter_1based < 1 || chapter_1based > MAX_CHAPTERS_PER_BOOK) return 50;
    
    // Look up verse count (chapter is 1-based, array is 0-indexed)
    uint16_t verse_count = catholic_bible_verse_counts[book_index][chapter_1based - 1];
    
    // If 0, means not yet implemented - use default placeholder
    if(verse_count == 0) {
        return 50; // Default placeholder for unimplemented books/chapters
    }
    
    return verse_count;
}

/* Verse text lookup - Phase 2.3: Uses storage adapter with fallback to hardcoded */
static const char* cb_get_verse_text(CatholicBibleApp* app, size_t book_index, uint16_t chapter, uint16_t verse) {
    if(!app) return "(Error: app is NULL)";
    
    // Try storage adapter first (Phase 2.2)
    if(storage_adapter_assets_available(&app->storage)) {
        // Allocate buffer if needed
        if(!app->current_verse_buffer) {
            app->current_verse_buffer = malloc(512); // Reasonable max verse length
            if(!app->current_verse_buffer) {
                return "(Error: failed to allocate buffer)";
            }
        }
        
        size_t bytes_read = storage_adapter_get_verse_text(
            &app->storage,
            book_index,
            chapter,
            verse,
            app->current_verse_buffer,
            512
        );
        
        if(bytes_read > 0) {
            return app->current_verse_buffer;
        }
        // Fall through to hardcoded if storage fails
    }
    
    // Fallback to hardcoded text (Phase 1.2)
    // Genesis (book_index 0), Chapter 1
    if(book_index == 0 && chapter == 1) {
        if(verse >= 1 && verse <= GENESIS_1_VERSE_COUNT) {
            return genesis1_verses[verse - 1]; // Convert 1-based to 0-based index
        }
    }

    // Placeholder for other books/chapters
    return "(Verse text not yet available. Add Bible assets to SD card.)";
}

/* ============================================================================
 * Submenu callback -> Custom event
 * ==========================================================================*/

static void catholic_bible_submenu_callback(void* context, uint32_t index) {
    CatholicBibleApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

/* ============================================================================
 * Navigation (Back) callback (SDK 1.4.3 style)
 * ==========================================================================*/

/* Return VIEW_NONE so ViewDispatcher calls navigation_callback (Back goes to scene manager). */
static uint32_t catholic_bible_submenu_previous_callback(void* context) {
    (void)context;
    return VIEW_NONE;
}

static bool catholic_bible_navigation_callback(void* context) {
    CatholicBibleApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static bool catholic_bible_custom_event_callback(void* context, uint32_t event) {
    CatholicBibleApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

/* ============================================================================
 * Scene: Menu
 * ==========================================================================*/

static void catholic_bible_scene_menu_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Catholic Bible");

    submenu_add_item(app->submenu, "Browse", MenuItemBrowse, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Last read", MenuItemLastRead, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Search", MenuItemSearch, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Missal", MenuItemMissal, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Rosary", MenuItemRosary, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Prayers", MenuItemPrayers, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Confession", MenuItemConfession, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Bookmarks", MenuItemBookmarks, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "History", MenuItemHistory, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "About", MenuItemAbout, catholic_bible_submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_menu_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case MenuItemBrowse:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBrowseBooks);
            return true;
        case MenuItemLastRead:
            if(history_manager_get_last_read(&app->history,
                    &app->selected_book_index, &app->selected_chapter, &app->selected_verse)) {
                scene_manager_next_scene(app->scene_manager, CatholicBibleSceneReader);
            } else {
                scene_manager_next_scene(app->scene_manager, CatholicBibleSceneHistory);
            }
            return true;
        case MenuItemSearch:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneSearch);
            return true;
        case MenuItemMissal:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneMissal);
            return true;
        case MenuItemRosary:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneRosary);
            return true;
        case MenuItemPrayers:
            scene_manager_next_scene(app->scene_manager, CatholicBibleScenePrayers);
            return true;
        case MenuItemConfession:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneConfession);
            return true;
        case MenuItemBookmarks:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBookmarks);
            return true;
        case MenuItemHistory:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneHistory);
            return true;
        case MenuItemAbout:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneAbout);
            return true;
        default:
            return false;
        }
    }

    return false;
}

static void catholic_bible_scene_menu_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

/* ============================================================================
 * Scene: Browse Books
 * ==========================================================================*/

static void catholic_bible_scene_browse_books_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Books");

    const size_t n = cb_books_count();
    for(size_t i = 0; i < n; i++) {
        submenu_add_item(app->submenu, cb_book_name(i), (uint32_t)i, catholic_bible_submenu_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_browse_books_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        app->selected_book_index = (size_t)event.event;
        app->chapter_page = 0;
        app->selected_chapter = 1;
        app->selected_verse = 1;
        scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBrowseChapters);
        return true;
    }

    return false;
}

static void catholic_bible_scene_browse_books_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

/* ============================================================================
 * Scene: Browse Chapters
 * ==========================================================================*/

static void catholic_bible_scene_browse_chapters_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);

    const char* book = cb_book_name(app->selected_book_index);
    submenu_set_header(app->submenu, book);

    const uint16_t chapters = cb_book_chapters(app->selected_book_index);
    if(chapters == 0) {
        submenu_add_item(app->submenu, "(No chapters)", 0, catholic_bible_submenu_callback, app);
        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
        return;
    }

    const uint16_t per_page = CHAPTERS_PER_PAGE;
    const uint16_t page_count = (chapters + per_page - 1) / per_page;

    if(app->chapter_page >= page_count) app->chapter_page = (page_count ? (page_count - 1) : 0);

    const uint16_t start = (uint16_t)(app->chapter_page * per_page + 1);
    uint16_t end = (uint16_t)(start + per_page - 1);
    if(end > chapters) end = chapters;

    // Prev page (if available)
    if(app->chapter_page > 0) {
        submenu_add_item(app->submenu, "< Prev", CH_EVT_PREV_PAGE, catholic_bible_submenu_callback, app);    }

    // Chapter items
    for(uint16_t c = start; c <= end; c++) {
        char label[24];
        snprintf(label, sizeof(label), "Chapter %u", (unsigned)c);
        submenu_add_item(app->submenu, label, CH_ENCODE_CHAPTER(c), catholic_bible_submenu_callback, app);
    }

    // Next page (if available)
    if(app->chapter_page + 1 < page_count) {
        submenu_add_item(app->submenu, "Next >", CH_EVT_NEXT_PAGE, catholic_bible_submenu_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_browse_chapters_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type != SceneManagerEventTypeCustom) return false;

    const uint32_t e = event.event;

    if(e == CH_EVT_PREV_PAGE) {
        if(app->chapter_page > 0) app->chapter_page--;
        scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBrowseChapters);
        return true;
    }

    if(e == CH_EVT_NEXT_PAGE) {
        app->chapter_page++;
        scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBrowseChapters);
        return true;
    }

    if(CH_IS_CHAPTER(e)) {
        app->selected_chapter = CH_DECODE_CHAPTER(e);
        app->selected_verse = 1;
        scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBrowseVerses);
        return true;
    }

    return false;
}

static void catholic_bible_scene_browse_chapters_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}


/* ============================================================================
 * Scene: Browse Verses (NEW scaffold)
 * ==========================================================================*/

static void catholic_bible_scene_browse_verses_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);

    // Header: "Book — Ch X"
    const char* book = cb_book_name(app->selected_book_index);
    char header[32];
    snprintf(header, sizeof(header), "Ch %u", (unsigned)app->selected_chapter);

    // Submenu header only supports one line; keep it simple:
    // Show book name; verse list indicates chapter.
    submenu_set_header(app->submenu, book);

    const uint16_t verses = cb_chapter_verses(app, app->selected_book_index, app->selected_chapter);

    // Also keep it sane; paging later.
    const uint16_t max_list = (verses > 80) ? 80 : verses;

    // Insert a “chapter label” item at top using a disabled-like pattern? (not supported)
    // Instead, first verse item can carry context.
    for(uint16_t v = 1; v <= max_list; v++) {
        char label[24];
        if(v == 1) {
            // Put the chapter context into the first label.
            snprintf(label, sizeof(label), "%s %u:%u", "Verse", (unsigned)app->selected_chapter, (unsigned)v);
        } else {
            snprintf(label, sizeof(label), "Verse %u", (unsigned)v);
        }
        submenu_add_item(app->submenu, label, VS_ENCODE_VERSE(v), catholic_bible_submenu_callback, app);
    }

    (void)header; // reserved for later when we add a better header/paging UI

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_browse_verses_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(VS_IS_VERSE(event.event)) {
            app->selected_verse = VS_DECODE_VERSE(event.event);
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneReader);
        return true;
        }
    }

    return false;
}

static void catholic_bible_scene_browse_verses_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
        widget_reset(app->widget);
}

/* ============================================================================
 * Custom Reader View with Scrolling
 * ==========================================================================*/

#define READER_HEADER_HEIGHT 16
#define READER_LINE_HEIGHT   2
#define READER_LEFT_MARGIN   4
#define READER_RIGHT_MARGIN  4
#define READER_TOP_MARGIN    2
#define READER_MAX_LINE_LEN  120

/* Draw one line only if it's in the visible band; return next Y (top of next line). */
static int32_t reader_draw_line(Canvas* canvas, const char* line, int32_t line_top, int32_t clip_top, int32_t clip_bottom, uint8_t font_height) {
    int32_t baseline = line_top + font_height;
    if(baseline < clip_top || line_top > clip_bottom) return line_top + font_height + READER_LINE_HEIGHT;
    canvas_draw_str(canvas, READER_LEFT_MARGIN, (uint8_t)baseline, line);
    return line_top + font_height + READER_LINE_HEIGHT;
}

/* Word-wrap and draw verse text. Sets app->reader_content_height. Never draws past width. */
static void reader_draw_text(Canvas* canvas, const char* text, int32_t scroll_y, uint8_t width, uint8_t height, CatholicBibleApp* app) {
    if(!text || !canvas) return;
    
    uint8_t font_height = canvas_current_font_height(canvas);
    int32_t content_top = READER_HEADER_HEIGHT + READER_TOP_MARGIN;
    int32_t current_y = content_top - scroll_y;
    uint8_t available_width = (width >= READER_LEFT_MARGIN + READER_RIGHT_MARGIN) ?
        (width - READER_LEFT_MARGIN - READER_RIGHT_MARGIN) : (width - 8);
    if(available_width > READER_MAX_LINE_LEN) available_width = READER_MAX_LINE_LEN;
    
    int32_t clip_top = READER_HEADER_HEIGHT;
    int32_t clip_bottom = (int32_t)height + font_height + READER_LINE_HEIGHT;
    
    char line[READER_MAX_LINE_LEN + 1];
    size_t line_len = 0;
    line[0] = '\0';
    
    const char* p = text;
    size_t text_remaining = strlen(p);
    if(text_remaining > 2000) text_remaining = 2000;
    
    while(text_remaining > 0) {
        /* Skip spaces and handle newlines */
        while(text_remaining > 0 && (*p == ' ' || *p == '\t')) { p++; text_remaining--; }
        if(text_remaining > 0 && *p == '\n') {
            line[line_len] = '\0';
            if(line_len > 0) {
                current_y = reader_draw_line(canvas, line, current_y, clip_top, clip_bottom, font_height);
                line_len = 0;
                line[0] = '\0';
            } else {
                current_y += font_height + READER_LINE_HEIGHT;
            }
            p++;
            text_remaining--;
            continue;
        }
        if(text_remaining == 0) break;
        
        /* Read next word (run of non-whitespace) */
        const char* word_start = p;
        while(text_remaining > 0 && *p != ' ' && *p != '\t' && *p != '\n') { p++; text_remaining--; }
        size_t word_len = (size_t)(p - word_start);
        if(word_len == 0) continue;
        if(word_len > READER_MAX_LINE_LEN) word_len = READER_MAX_LINE_LEN;
        
        /* Measure word (or first chunk if word too long) */
        char word_buf[READER_MAX_LINE_LEN + 1];
        if(word_len >= sizeof(word_buf)) word_len = sizeof(word_buf) - 1;
        memcpy(word_buf, word_start, word_len);
        word_buf[word_len] = '\0';
        
        uint16_t word_width = canvas_string_width(canvas, word_buf);
        uint16_t space_width = (line_len > 0) ? canvas_string_width(canvas, " ") : 0;
        uint16_t line_width = canvas_string_width(canvas, line);
        uint16_t would_be = line_width + space_width + word_width;
        
        /* If word alone exceeds width, break into chunks that fit */
        if(word_width > available_width) {
            line[line_len] = '\0';
            if(line_len > 0) {
                current_y = reader_draw_line(canvas, line, current_y, clip_top, clip_bottom, font_height);
                line_len = 0;
                line[0] = '\0';
            }
            for(size_t chunk_start = 0; chunk_start < word_len; ) {
                size_t n = 0;
                for(size_t k = 1; chunk_start + k <= word_len; k++) {
                    line[0] = '\0';
                    memcpy(line, word_start + chunk_start, k);
                    line[k] = '\0';
                    if(canvas_string_width(canvas, line) <= (uint16_t)available_width)
                        n = k;
                    else
                        break;
                }
                if(n == 0) n = 1;
                line[n] = '\0';
                current_y = reader_draw_line(canvas, line, current_y, clip_top, clip_bottom, font_height);
                chunk_start += n;
            }
            line_len = 0;
            line[0] = '\0';
            continue;
        }
        
        /* If adding this word would overflow, flush current line first */
        if(line_len > 0 && would_be > available_width) {
            line[line_len] = '\0';
            current_y = reader_draw_line(canvas, line, current_y, clip_top, clip_bottom, font_height);
            line_len = 0;
            line[0] = '\0';
        }
        
        /* Append word to line (with space if not first) */
        if(line_len > 0) {
            line[line_len++] = ' ';
            line[line_len] = '\0';
        }
        for(size_t i = 0; i < word_len && line_len < READER_MAX_LINE_LEN; i++)
            line[line_len++] = word_buf[i];
        line[line_len] = '\0';
    }
    
    if(line_len > 0) {
        current_y = reader_draw_line(canvas, line, current_y, clip_top, clip_bottom, font_height);
    }
    
    if(app) {
        int32_t ch = current_y - content_top;
        app->reader_content_height = (ch > 0) ? ch : 0;
    }
}

/* Reader viewport draw callback */
static void reader_viewport_draw_callback(Canvas* canvas, void* context) {
    CatholicBibleApp* app = context;
    
    if(!app || !canvas) return;
    
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    
    /* Header: "Book C:V" and optional "(Bookmarked)" (Phase 4.3) */
    const char* book = cb_book_name(app->selected_book_index);
    if(book) {
        char header[48];
        snprintf(header, sizeof(header), "%s %u:%u", book,
                 (unsigned)app->selected_chapter,
                 (unsigned)app->selected_verse);
        canvas_draw_str(canvas, READER_LEFT_MARGIN, 10, header);
        if(bookmark_manager_is_bookmarked(&app->bookmarks, app->selected_book_index,
                app->selected_chapter, app->selected_verse)) {
            canvas_draw_str(canvas, 70, 10, "(BM)");
        }
    }
    
    /* Separator below header; content is strictly below this */
    canvas_draw_line(canvas, 0, READER_HEADER_HEIGHT - 1, 128, READER_HEADER_HEIGHT - 1);
    
    /* Verse body: word-wrapped, clipped to content area */
    canvas_set_font(canvas, FontKeyboard);
    
    if(app->current_verse_text && strlen(app->current_verse_text) > 0) {
        reader_draw_text(canvas, app->current_verse_text, app->scroll_offset,
                        128, 64 - READER_HEADER_HEIGHT, app);
    } else {
        if(app) app->reader_content_height = 0;
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, READER_LEFT_MARGIN, READER_HEADER_HEIGHT + 14, "No text");
    }
}

/* Reader ViewPort input callback: infinite scroll (Up/Down = prev/next verse when at edge). */
static void reader_viewport_input_callback(InputEvent* event, void* context) {
    CatholicBibleApp* app = context;
    if(!app || !event || !app->reader_viewport) return;
    
    if(event->type != InputTypeShort && event->type != InputTypeRepeat) return;
    
    const uint8_t visible_height = 64 - READER_HEADER_HEIGHT;
    int32_t max_scroll = app->reader_content_height - visible_height;
    if(max_scroll < 0) max_scroll = 0;
    
    switch(event->key) {
    case InputKeyBack:
        if(app->view_dispatcher)
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_BACK);
        break;
    case InputKeyUp:
        if(app->scroll_offset > 0) {
            app->scroll_offset -= 10;
            if(app->scroll_offset < 0) app->scroll_offset = 0;
            view_port_update(app->reader_viewport);
        } else if(app->view_dispatcher) {
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_PREV_VERSE);
        }
        break;
    case InputKeyDown:
        if(app->scroll_offset < max_scroll) {
            app->scroll_offset += 10;
            if(app->scroll_offset > max_scroll) app->scroll_offset = max_scroll;
            view_port_update(app->reader_viewport);
        } else if(app->view_dispatcher) {
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_NEXT_VERSE);
        }
        break;
    case InputKeyOk:
        if(app->view_dispatcher)
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_TOGGLE_BM);
        break;
    case InputKeyLeft:
        if(app->view_dispatcher)
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_PREV_VERSE);
        break;
    case InputKeyRight:
        if(app->view_dispatcher)
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_NEXT_VERSE);
        break;
    default:
        break;
    }
}

/* ============================================================================
 * Scene: Reader
 * ==========================================================================*/

static void catholic_bible_scene_reader_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    if(!app) return;

    // Get verse text
    app->current_verse_text = cb_get_verse_text(
        app,
        app->selected_book_index,
        app->selected_chapter,
        app->selected_verse
    );
    
    // Track in history (Phase 4.2)
    history_manager_add_entry(
        &app->history,
        app->selected_book_index,
        app->selected_chapter,
        app->selected_verse
    );
    
    // Update last-read verse (Phase 4.2)
    history_manager_set_last_read(
        &app->history,
        app->selected_book_index,
        app->selected_chapter,
        app->selected_verse
    );
    
    app->scroll_offset = 0;
    app->reader_content_height = 0;

    // Show reader via ViewPort only (add to GUI so it actually draws)
    if(app->gui && app->reader_viewport) {
        gui_add_view_port(app->gui, app->reader_viewport, GuiLayerFullscreen);
        view_port_update(app->reader_viewport);
    }
}

static bool catholic_bible_scene_reader_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        uint16_t max_verses = cb_chapter_verses(app, app->selected_book_index, app->selected_chapter);
        if(max_verses == 0) max_verses = 1;

        if(event.event == READER_EVT_BACK) {
            scene_manager_previous_scene(app->scene_manager);
            return true;
        }
        if(event.event == READER_EVT_PREV_VERSE) {
            if(app->selected_verse > 1) {
                app->selected_verse--;
                scene_manager_next_scene(app->scene_manager, CatholicBibleSceneReader);
            }
            return true;
        }
        if(event.event == READER_EVT_NEXT_VERSE) {
            if(app->selected_verse < max_verses) {
                app->selected_verse++;
                scene_manager_next_scene(app->scene_manager, CatholicBibleSceneReader);
            }
            return true;
        }
        if(event.event == READER_EVT_TOGGLE_BM) {
            int idx = bookmark_manager_find(&app->bookmarks, app->selected_book_index,
                    app->selected_chapter, app->selected_verse);
            if(idx >= 0) {
                bookmark_manager_delete(&app->bookmarks, (size_t)idx);
            } else {
                char name[BOOKMARK_NAME_MAX_LEN];
                const char* book = cb_book_name(app->selected_book_index);
                snprintf(name, sizeof(name), "%s %u:%u", book ? book : "?", 
                         (unsigned)app->selected_chapter, (unsigned)app->selected_verse);
                bookmark_manager_add(&app->bookmarks, app->selected_book_index,
                        app->selected_chapter, app->selected_verse, name);
            }
            view_port_update(app->reader_viewport);
            return true;
        }
    }

    return false;
}

static void catholic_bible_scene_reader_on_exit(void* context) {
    CatholicBibleApp* app = context;
    if(app->gui && app->reader_viewport)
        gui_remove_view_port(app->gui, app->reader_viewport);
    app->current_verse_text = NULL;
    app->scroll_offset = 0;
    app->reader_content_height = 0;
    /* Persist history when leaving reader (Phase 4.2) */
    history_manager_save(&app->history);
}

/* ============================================================================
 * Scene: Search (placeholder)
 * ==========================================================================*/

static void catholic_bible_scene_search_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 4, 8, AlignLeft, AlignTop, FontPrimary, "Search");
    widget_add_string_element(app->widget, 4, 22, AlignLeft, AlignTop, FontSecondary,
                              "Phase 3: full-text search");
    widget_add_string_element(app->widget, 4, 36, AlignLeft, AlignTop, FontSecondary,
                              "requires index build");
    widget_add_string_element(app->widget, 4, 50, AlignLeft, AlignTop, FontSecondary,
                              "(tools). Coming soon.");

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
}

static bool catholic_bible_scene_search_on_event(void* context, SceneManagerEvent event) {
    (void)context;
    (void)event;
    return false;
}

static void catholic_bible_scene_search_on_exit(void* context) {
    CatholicBibleApp* app = context;
    widget_reset(app->widget);
}

/* ============================================================================
 * Scene: About (placeholder)
 * ==========================================================================*/

static void catholic_bible_scene_about_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 4, 8, AlignLeft, AlignTop, FontPrimary, "Catholic Bible");
    widget_add_string_element(app->widget, 4, 20, AlignLeft, AlignTop, FontSecondary, "Douay-Rheims, offline");
    widget_add_string_element(app->widget, 4, 32, AlignLeft, AlignTop, FontSecondary,
                              "Browse / Last read / BM / History");
    widget_add_string_element(app->widget, 4, 44, AlignLeft, AlignTop, FontSecondary,
                              "SD: /apps_data/bible/");

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
}

static bool catholic_bible_scene_about_on_event(void* context, SceneManagerEvent event) {
    (void)context;
    (void)event;
    return false;
}

static void catholic_bible_scene_about_on_exit(void* context) {
    CatholicBibleApp* app = context;
    widget_reset(app->widget);
}

/* ============================================================================
 * Scene: Bookmarks (Phase 4.3) - submenu, select opens reader
 * ==========================================================================*/

#define BM_EVT_BASE 0xB0000000u

static void catholic_bible_scene_bookmarks_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Bookmarks");
    
    size_t count = bookmark_manager_count(&app->bookmarks);
    if(count == 0) {
        submenu_add_item(app->submenu, "(No bookmarks)", 0, catholic_bible_submenu_callback, app);
        submenu_add_item(app->submenu, "OK in reader to add", 0xFF, catholic_bible_submenu_callback, app);
    } else {
        for(size_t i = 0; i < count; i++) {
            const Bookmark* b = bookmark_manager_get(&app->bookmarks, i);
            if(b && b->valid) {
                const char* label = (b->name[0] != '\0') ? b->name : "?";
                submenu_add_item(app->submenu, label, (uint32_t)(BM_EVT_BASE | i), catholic_bible_submenu_callback, app);
            }
        }
    }
    
    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_bookmarks_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;
    
    if(event.type == SceneManagerEventTypeCustom && (event.event & 0xB0000000u) == BM_EVT_BASE) {
        size_t index = (size_t)(event.event & 0xFFu);
        if(index >= bookmark_manager_count(&app->bookmarks)) return true;
        const Bookmark* b = bookmark_manager_get(&app->bookmarks, index);
        if(b && b->valid) {
            app->selected_book_index = b->book_index;
            app->selected_chapter = b->chapter;
            app->selected_verse = b->verse;
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneReader);
        }
        return true;
    }
    
    return false;
}

static void catholic_bible_scene_bookmarks_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

/* ============================================================================
 * Scene: History (Phase 4.3) - submenu, select opens reader
 * ==========================================================================*/

#define HIST_EVT_BASE 0xD0000000u

static void catholic_bible_scene_history_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Recent");
    
    size_t count = history_manager_count(&app->history);
    if(count == 0) {
        submenu_add_item(app->submenu, "(No history yet)", 0, catholic_bible_submenu_callback, app);
        submenu_add_item(app->submenu, "Read to build history", 0xFF, catholic_bible_submenu_callback, app);
    } else {
        for(size_t i = 0; i < count; i++) {
            const HistoryEntry* e = history_manager_get_entry(&app->history, i);
            if(e && e->valid) {
                char label[32];
                const char* book = cb_book_name(e->book_index);
                snprintf(label, sizeof(label), "%s %u:%u", book ? book : "?", 
                         (unsigned)e->chapter, (unsigned)e->verse);
                submenu_add_item(app->submenu, label, (uint32_t)(HIST_EVT_BASE | i), catholic_bible_submenu_callback, app);
            }
        }
    }
    
    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_history_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;
    
    if(event.type == SceneManagerEventTypeCustom && (event.event & 0xD0000000u) == HIST_EVT_BASE) {
        size_t index = (size_t)(event.event & 0xFFu);
        if(index >= history_manager_count(&app->history)) return true;
        const HistoryEntry* e = history_manager_get_entry(&app->history, index);
        if(e && e->valid) {
            app->selected_book_index = e->book_index;
            app->selected_chapter = e->chapter;
            app->selected_verse = e->verse;
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneReader);
        }
        return true;
    }
    
    return false;
}

static void catholic_bible_scene_history_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

/* ============================================================================
 * Scene: Missal (Phase 6.1)
 * ==========================================================================*/

static void catholic_bible_scene_missal_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Roman Catholic Missal");
    
    // Submenu options for Missal features
    submenu_add_item(app->submenu, "Today's Mass", 0, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Browse by Date", 1, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Liturgical Calendar", 2, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Mass Prayers", 3, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Mass Responses", 4, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Readings Search", 5, catholic_bible_submenu_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_missal_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;
    
    if(event.type == SceneManagerEventTypeCustom) {
        // Placeholder: Will navigate to specific Missal features
        // For now, show placeholder widget
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, "Missal Feature");
        widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary,
                                  "Coming soon.");
        widget_add_string_element(app->widget, 4, 48, AlignLeft, AlignTop, FontSecondary,
                                  "Phase 6.1");
        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
        return true;
    }
    
    return false;
}

static void catholic_bible_scene_missal_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
    widget_reset(app->widget);
}

/* ============================================================================
 * Scene: Rosary (Phase 6.2)
 * ==========================================================================*/

static void catholic_bible_scene_rosary_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Rosary Guide");
    
    // Submenu options for Rosary features
    submenu_add_item(app->submenu, "How to Pray Rosary", 0, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Joyful Mysteries", 1, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Sorrowful Mysteries", 2, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Glorious Mysteries", 3, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Luminous Mysteries", 4, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Rosary Prayers", 5, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Scriptural Rosary", 6, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Rosary Intentions", 7, catholic_bible_submenu_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_rosary_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;
    
    if(event.type == SceneManagerEventTypeCustom) {
        // Placeholder: Will navigate to specific Rosary features
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, "Rosary Feature");
        widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary,
                                  "Coming soon.");
        widget_add_string_element(app->widget, 4, 48, AlignLeft, AlignTop, FontSecondary,
                                  "Phase 6.2");
        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
        return true;
    }
    
    return false;
}

static void catholic_bible_scene_rosary_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
    widget_reset(app->widget);
}

/* ============================================================================
 * Scene: Common Prayers (Phase 6.3)
 * ==========================================================================*/

static void catholic_bible_scene_prayers_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Common Prayers");
    
    // Submenu with all common prayers
    submenu_add_item(app->submenu, "Sign of the Cross", 0, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Prayer of Holy Spirit", 1, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Act of Contrition", 2, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Lord's Prayer", 3, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Hail Mary", 4, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Glory Be", 5, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Guardian Angel", 6, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Hail Holy Queen", 7, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Memorare", 8, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "St. Francis Prayer", 9, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "St. Michael Prayer", 10, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Apostles' Creed", 11, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Prayer Before Meals", 12, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Morning Offering", 13, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Evening Prayer", 14, catholic_bible_submenu_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_prayers_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;
    
    if(event.type == SceneManagerEventTypeCustom) {
        // Placeholder: Will show prayer text
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, "Prayer");
        widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary,
                                  "Coming soon.");
        widget_add_string_element(app->widget, 4, 48, AlignLeft, AlignTop, FontSecondary,
                                  "Phase 6.3");
        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
        return true;
    }
    
    return false;
}

static void catholic_bible_scene_prayers_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
    widget_reset(app->widget);
}

/* ============================================================================
 * Scene: Confession Preparation (Phase 6.4)
 * ==========================================================================*/

static void catholic_bible_scene_confession_on_enter(void* context) {
    CatholicBibleApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Confession Prep");
    
    // Submenu options for Confession preparation
    submenu_add_item(app->submenu, "Confession Guide", 0, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "10 Commandments", 1, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "7 Deadly Sins", 2, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Examination Q&A", 3, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Act of Contrition", 4, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Confession Prayers", 5, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Tips for Nervous", 6, catholic_bible_submenu_callback, app);
    submenu_add_item(app->submenu, "Post-Confession", 7, catholic_bible_submenu_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool catholic_bible_scene_confession_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;
    
    if(event.type == SceneManagerEventTypeCustom) {
        // Placeholder: Will show Confession preparation content
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, "Confession Prep");
        widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary,
                                  "Coming soon.");
        widget_add_string_element(app->widget, 4, 48, AlignLeft, AlignTop, FontSecondary,
                                  "Phase 6.4");
        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
        return true;
    }
    
    return false;
}

static void catholic_bible_scene_confession_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
    widget_reset(app->widget);
}

/* ============================================================================
 * Scene handlers table (SDK 1.4.3 compatible)
 * ==========================================================================*/

static void (*const catholic_bible_on_enter_handlers[])(void*) = {
    catholic_bible_scene_menu_on_enter,
    catholic_bible_scene_browse_books_on_enter,
    catholic_bible_scene_browse_chapters_on_enter,
    catholic_bible_scene_browse_verses_on_enter,
    catholic_bible_scene_reader_on_enter,
    catholic_bible_scene_search_on_enter,
    catholic_bible_scene_missal_on_enter,
    catholic_bible_scene_rosary_on_enter,
    catholic_bible_scene_prayers_on_enter,
    catholic_bible_scene_confession_on_enter,
    catholic_bible_scene_bookmarks_on_enter,
    catholic_bible_scene_history_on_enter,
    catholic_bible_scene_about_on_enter,
};

static bool (*const catholic_bible_on_event_handlers[])(void*, SceneManagerEvent) = {
    catholic_bible_scene_menu_on_event,
    catholic_bible_scene_browse_books_on_event,
    catholic_bible_scene_browse_chapters_on_event,
    catholic_bible_scene_browse_verses_on_event,
    catholic_bible_scene_reader_on_event,
    catholic_bible_scene_search_on_event,
    catholic_bible_scene_missal_on_event,
    catholic_bible_scene_rosary_on_event,
    catholic_bible_scene_prayers_on_event,
    catholic_bible_scene_confession_on_event,
    catholic_bible_scene_bookmarks_on_event,
    catholic_bible_scene_history_on_event,
    catholic_bible_scene_about_on_event,
};

static void (*const catholic_bible_on_exit_handlers[])(void*) = {
    catholic_bible_scene_menu_on_exit,
    catholic_bible_scene_browse_books_on_exit,
    catholic_bible_scene_browse_chapters_on_exit,
    catholic_bible_scene_browse_verses_on_exit,
    catholic_bible_scene_reader_on_exit,
    catholic_bible_scene_search_on_exit,
    catholic_bible_scene_missal_on_exit,
    catholic_bible_scene_rosary_on_exit,
    catholic_bible_scene_prayers_on_exit,
    catholic_bible_scene_confession_on_exit,
    catholic_bible_scene_bookmarks_on_exit,
    catholic_bible_scene_history_on_exit,
    catholic_bible_scene_about_on_exit,
};

static const SceneManagerHandlers catholic_bible_scene_handlers = {
    .on_enter_handlers = catholic_bible_on_enter_handlers,
    .on_event_handlers = catholic_bible_on_event_handlers,
    .on_exit_handlers = catholic_bible_on_exit_handlers,
    .scene_num = CatholicBibleSceneCount,
};

/* ============================================================================
 * TextBox input callback for Reader scene (Left/Right navigation)
 * ==========================================================================*/
/* NOTE: TextBox scrolling with Up/Down may not work properly.
 * If scrolling proves inadequate for long verses, consider implementing
 * a custom reader view with proper scroll support. */

static bool catholic_bible_reader_textbox_input_callback(InputEvent* event, void* context) {
    CatholicBibleApp* app = context;

    // Only handle short press events for Left/Right keys
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_PREV_VERSE);
            return true;
        }

        if(event->key == InputKeyRight) {
            view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_NEXT_VERSE);
            return true;
        }
    }

    // Let TextBox handle other inputs (Up/Down for scrolling)
    // Note: Using custom ViewPort-based reader view for proper scrolling control
    return false;
}

/* ============================================================================
 * App lifecycle
 * ==========================================================================*/

static CatholicBibleApp* catholic_bible_app_alloc(void) {
    CatholicBibleApp* app = malloc(sizeof(CatholicBibleApp));
    if(!app) {
        return NULL; // Memory allocation failed
    }
    memset(app, 0, sizeof(CatholicBibleApp));

    app->gui = furi_record_open(RECORD_GUI);
    if(!app->gui) {
        free(app);
        return NULL; // Failed to open GUI record
    }

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&catholic_bible_scene_handlers, app);

    app->submenu = submenu_alloc();
    app->widget = widget_alloc();
    app->text_box = text_box_alloc();

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, catholic_bible_navigation_callback);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, catholic_bible_custom_event_callback);

    // Attach to GUI (fullscreen)
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Register views
    View* submenu_view = submenu_get_view(app->submenu);
    view_set_previous_callback(submenu_view, catholic_bible_submenu_previous_callback);
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewSubmenu, submenu_view);
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewWidget, widget_get_view(app->widget));
    
    // Register TextBox view with input callback for navigation
    View* text_box_view = text_box_get_view(app->text_box);
    view_set_context(text_box_view, app);
    view_set_input_callback(text_box_view, catholic_bible_reader_textbox_input_callback);
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewTextBox, text_box_view);

    // Reader: single ViewPort added to GUI when in reader scene (fixes blank screen)
    app->reader_viewport = view_port_alloc();
    view_port_draw_callback_set(app->reader_viewport, reader_viewport_draw_callback, app);
    view_port_input_callback_set(app->reader_viewport, reader_viewport_input_callback, app);
    view_port_enabled_set(app->reader_viewport, true);

    // Initialize storage adapter (Phase 2.2)
    storage_adapter_init(&app->storage);
    
    // Initialize bookmark and history managers (Phase 4)
    if(!bookmark_manager_init(&app->bookmarks)) {
        // Log error but continue - bookmarks are optional
        FURI_LOG_E("CatholicBible", "Failed to initialize bookmark manager");
    }
    if(!history_manager_init(&app->history)) {
        // Log error but continue - history is optional
        FURI_LOG_E("CatholicBible", "Failed to initialize history manager");
    }

    // Start at menu
    scene_manager_next_scene(app->scene_manager, CatholicBibleSceneMenu);

    return app;
}

static void catholic_bible_app_free(CatholicBibleApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewTextBox);

    submenu_free(app->submenu);
    widget_free(app->widget);
    text_box_free(app->text_box);
    
    if(app->reader_viewport) {
        view_port_free(app->reader_viewport);
    }
    
    // Free storage adapter (Phase 2.2)
    storage_adapter_free(&app->storage);
    
    // Free bookmark and history managers (Phase 4)
    bookmark_manager_free(&app->bookmarks);
    history_manager_free(&app->history);
    
    // Free verse buffer
    if(app->current_verse_buffer) {
        free(app->current_verse_buffer);
        app->current_verse_buffer = NULL;
    }

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t catholic_bible_app(void* p) {
    (void)p;

    CatholicBibleApp* app = catholic_bible_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    catholic_bible_app_free(app);

    return 0;
}
