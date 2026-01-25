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

#define READER_EVT_PREV_VERSE 0x91000001u
#define READER_EVT_NEXT_VERSE 0x91000002u

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

static const char* const genesis1_verses[GENESIS_1_VERSE_COUNT] = {
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

/* Verse count lookup - Phase 1.3: Returns real verse counts from books_meta */
static uint16_t cb_chapter_verses(size_t book_index, uint16_t chapter_1based) {
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

/* Verse text lookup - Phase 1.2: Returns actual verses for Genesis 1, placeholder for others */
static const char* cb_get_verse_text(size_t book_index, uint16_t chapter, uint16_t verse) {
    // Genesis (book_index 0), Chapter 1
    if(book_index == 0 && chapter == 1) {
        if(verse >= 1 && verse <= GENESIS_1_VERSE_COUNT) {
            return genesis1_verses[verse - 1]; // Convert 1-based to 0-based index
        }
    }

    // Placeholder for other books/chapters (to be implemented in Phase 2)
    return "(Verse text not yet available. Coming in Phase 2.)";
}

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
    CatholicBibleSceneAbout,
    CatholicBibleSceneCount,
} CatholicBibleSceneId;

typedef enum {
    MenuItemBrowse = 0,
    MenuItemSearch,
    MenuItemAbout,
} MenuItemId;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Submenu* submenu;
    Widget* widget;
    TextBox* text_box;
    ViewPort* reader_viewport; // ViewPort for custom drawing
    View* reader_view; // View wrapper for ViewDispatcher

    // Browse state
    size_t selected_book_index;
    uint16_t selected_chapter; // 1-based
    uint16_t selected_verse;   // 1-based
    uint16_t chapter_page;
    
    // Reader view state
    int32_t scroll_offset; // Scroll position (pixels)
    const char* current_verse_text; // Current verse text being displayed
} CatholicBibleApp;

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
    submenu_add_item(app->submenu, "Search", MenuItemSearch, catholic_bible_submenu_callback, app);
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
        case MenuItemSearch:
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneSearch);
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

    const uint16_t verses = cb_chapter_verses(app->selected_book_index, app->selected_chapter);

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

#define READER_HEADER_HEIGHT 28
#define READER_LINE_SPACING 10
#define READER_LEFT_MARGIN 4
#define READER_RIGHT_MARGIN 4
#define READER_TOP_MARGIN 4

/* Draw wrapped text with scroll offset - simplified and safer version */
static void reader_draw_text(Canvas* canvas, const char* text, int32_t scroll_y, uint8_t width, uint8_t height) {
    if(!text || !canvas) return;
    
    uint8_t font_height = canvas_current_font_height(canvas);
    // Start text at top of text area (below header)
    int32_t current_y = READER_HEADER_HEIGHT + READER_TOP_MARGIN - scroll_y;
    uint8_t available_width = width - READER_LEFT_MARGIN - READER_RIGHT_MARGIN;
    
    // Simple approach: draw text character by character with word wrapping
    // This avoids complex pointer arithmetic that could cause BusFault
    
    char line[128];
    size_t line_pos = 0;
    size_t text_len = strlen(text);
    if(text_len > 1000) text_len = 1000; // Safety limit
    
    for(size_t i = 0; i < text_len && current_y < (int32_t)(height + font_height); i++) {
        char c = text[i];
        
        // Handle newlines
        if(c == '\n') {
            line[line_pos] = '\0';
            if(current_y + font_height >= 0 && current_y < (int32_t)height && line_pos > 0) {
                canvas_draw_str(canvas, READER_LEFT_MARGIN, current_y + font_height, line);
            }
            line_pos = 0;
            current_y += font_height + READER_LINE_SPACING;
            continue;
        }
        
        // Add character to line if space available
        if(line_pos < sizeof(line) - 1) {
            line[line_pos++] = c;
            line[line_pos] = '\0';
            
            // Check if line would overflow (simple check on space boundaries)
            if(c == ' ' || line_pos == sizeof(line) - 1) {
                uint16_t line_width = canvas_string_width(canvas, line);
                
                // If line too long and we're at a space, wrap it
                if(line_width > available_width && c == ' ') {
                    // Draw line without last space
                    line[line_pos - 1] = '\0';
                    if(current_y + font_height >= 0 && current_y < (int32_t)height) {
                        canvas_draw_str(canvas, READER_LEFT_MARGIN, current_y + font_height, line);
                    }
                    line_pos = 0;
                    current_y += font_height + READER_LINE_SPACING;
                }
            }
        }
    }
    
    // Draw remaining line (if any characters were accumulated)
    if(line_pos > 0) {
        line[line_pos] = '\0';
        // Only draw if line is visible
        if(current_y + font_height >= 0 && current_y < (int32_t)(height + font_height)) {
            canvas_draw_str(canvas, READER_LEFT_MARGIN, current_y + font_height, line);
        }
    }
}

/* Reader viewport draw callback */
static void reader_viewport_draw_callback(Canvas* canvas, void* context) {
    CatholicBibleApp* app = context;
    
    if(!app || !canvas) return;
    
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    
    // Draw header: Book Chapter:Verse (with safety checks)
    const char* book = cb_book_name(app->selected_book_index);
    if(book) {
        char header[64];
        snprintf(header, sizeof(header), "%s %u:%u", book, 
                 (unsigned)app->selected_chapter, 
                 (unsigned)app->selected_verse);
        canvas_draw_str(canvas, READER_LEFT_MARGIN, 12, header);
    }
    
    // Draw separator line
    canvas_draw_line(canvas, 0, READER_HEADER_HEIGHT - 2, 128, READER_HEADER_HEIGHT - 2);
    
    // Draw verse text
    canvas_set_font(canvas, FontKeyboard);
    
    if(app->current_verse_text && strlen(app->current_verse_text) > 0) {
        uint8_t viewport_width = 128;
        uint8_t viewport_height = 64 - READER_HEADER_HEIGHT;
        reader_draw_text(canvas, app->current_verse_text, app->scroll_offset, 
                        viewport_width, viewport_height);
    } else {
        // Show placeholder if no text loaded yet
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, READER_LEFT_MARGIN, READER_HEADER_HEIGHT + 20, "No text");
    }
}

/* Reader viewport input callback - ViewPort version returns void */
static void reader_viewport_input_callback(InputEvent* event, void* context) {
    CatholicBibleApp* app = context;
    
    if(!app || !event) return;
    
    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        if(event->key == InputKeyUp) {
            // Scroll up
            if(app->scroll_offset > 0) {
                app->scroll_offset -= 10;
                if(app->scroll_offset < 0) app->scroll_offset = 0;
                view_port_update(app->reader_viewport);
            }
            return;
        }
        
        if(event->key == InputKeyDown) {
            // Scroll down
            app->scroll_offset += 10;
            view_port_update(app->reader_viewport);
            return;
        }
        
        if(event->key == InputKeyLeft) {
            // Previous verse
            if(app->view_dispatcher) {
                view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_PREV_VERSE);
            }
            return;
        }
        
        if(event->key == InputKeyRight) {
            // Next verse
            if(app->view_dispatcher) {
                view_dispatcher_send_custom_event(app->view_dispatcher, READER_EVT_NEXT_VERSE);
            }
            return;
        }
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
        app->selected_book_index,
        app->selected_chapter,
        app->selected_verse
    );
    
    // Reset scroll to top when entering new verse
    app->scroll_offset = 0;

    // Switch to custom reader view and trigger update
    if(app->view_dispatcher) {
        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewReader);
    }
    
    // Force ViewPort update to redraw with new verse text
    if(app->reader_viewport) {
        view_port_update(app->reader_viewport);
    }
}

static bool catholic_bible_scene_reader_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        uint16_t max_verses = cb_chapter_verses(app->selected_book_index, app->selected_chapter);
        if(max_verses == 0) max_verses = 1;

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
    }

    return false;
}

static void catholic_bible_scene_reader_on_exit(void* context) {
    CatholicBibleApp* app = context;
    app->current_verse_text = NULL;
    app->scroll_offset = 0;
}

/* ============================================================================
 * Scene: Search (placeholder)
 * ==========================================================================*/

static void catholic_bible_scene_search_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, "Search");
    widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary,
                              "Scaffold only.");
    widget_add_string_element(app->widget, 4, 48, AlignLeft, AlignTop, FontSecondary,
                              "Next: input + index.");

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
    widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, "About");
    widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary,
                              "Flipper Catholic Bible");
    widget_add_string_element(app->widget, 4, 48, AlignLeft, AlignTop, FontSecondary,
                              "Browse: Books->Ch->Verses");

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
 * Scene handlers table (SDK 1.4.3 compatible)
 * ==========================================================================*/

static void (*const catholic_bible_on_enter_handlers[])(void*) = {
    catholic_bible_scene_menu_on_enter,
    catholic_bible_scene_browse_books_on_enter,
    catholic_bible_scene_browse_chapters_on_enter,
    catholic_bible_scene_browse_verses_on_enter,
    catholic_bible_scene_reader_on_enter,
    catholic_bible_scene_search_on_enter,
    catholic_bible_scene_about_on_enter,
};

static bool (*const catholic_bible_on_event_handlers[])(void*, SceneManagerEvent) = {
    catholic_bible_scene_menu_on_event,
    catholic_bible_scene_browse_books_on_event,
    catholic_bible_scene_browse_chapters_on_event,
    catholic_bible_scene_browse_verses_on_event,
    catholic_bible_scene_reader_on_event,
    catholic_bible_scene_search_on_event,
    catholic_bible_scene_about_on_event,
};

static void (*const catholic_bible_on_exit_handlers[])(void*) = {
    catholic_bible_scene_menu_on_exit,
    catholic_bible_scene_browse_books_on_exit,
    catholic_bible_scene_browse_chapters_on_exit,
    catholic_bible_scene_browse_verses_on_exit,
    catholic_bible_scene_reader_on_exit,
    catholic_bible_scene_search_on_exit,
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
    memset(app, 0, sizeof(CatholicBibleApp));

    app->gui = furi_record_open(RECORD_GUI);

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
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewWidget, widget_get_view(app->widget));
    
    // Register TextBox view with input callback for navigation
    View* text_box_view = text_box_get_view(app->text_box);
    view_set_context(text_box_view, app);
    view_set_input_callback(text_box_view, catholic_bible_reader_textbox_input_callback);
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewTextBox, text_box_view);

    // Create custom reader view using ViewPort (more reliable for custom drawing)
    app->reader_viewport = view_port_alloc();
    view_port_draw_callback_set(app->reader_viewport, reader_viewport_draw_callback, app);
    view_port_input_callback_set(app->reader_viewport, reader_viewport_input_callback, app);
    view_port_enabled_set(app->reader_viewport, true);
    
    // Wrap ViewPort in View for ViewDispatcher compatibility
    app->reader_view = view_alloc();
    view_set_view_port(app->reader_view, app->reader_viewport);
    view_set_context(app->reader_view, app);
    view_set_orientation(app->reader_view, ViewOrientationVertical);
    
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewReader, app->reader_view);

    // Start at menu
    scene_manager_next_scene(app->scene_manager, CatholicBibleSceneMenu);

    return app;
}

static void catholic_bible_app_free(CatholicBibleApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewTextBox);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewReader);

    submenu_free(app->submenu);
    widget_free(app->widget);
    text_box_free(app->text_box);
    
    if(app->reader_viewport) {
        view_port_free(app->reader_viewport);
    }
    
    if(app->reader_view) {
        view_free(app->reader_view);
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
