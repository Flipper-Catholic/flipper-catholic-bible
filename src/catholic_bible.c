#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>

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

/* Verse count stub (lightweight, no drama).
 * Next step later: replace with real per-chapter verse counts / index offsets. */
static uint16_t cb_chapter_verses_stub(size_t book_index, uint16_t chapter_1based) {
    (void)book_index;
    (void)chapter_1based;
    return 50;
}

/* ============================================================================
 * App definitions
 * ==========================================================================*/

typedef enum {
    CatholicBibleViewSubmenu = 0,
    CatholicBibleViewWidget,
} CatholicBibleViewId;

typedef enum {
    CatholicBibleSceneMenu = 0,
    CatholicBibleSceneBrowseBooks,
    CatholicBibleSceneBrowseChapters,
    CatholicBibleSceneBrowseVerses,
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

    // Browse state
    size_t selected_book_index;
    uint16_t selected_chapter; // 1-based
    uint16_t selected_verse;   // 1-based
    uint16_t chapter_page;
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

    const uint16_t verses = cb_chapter_verses_stub(app->selected_book_index, app->selected_chapter);

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
        app->selected_verse = (uint16_t)event.event;

        // Confirmation scaffold on Widget view (next step: Reader scene with actual text)
        widget_reset(app->widget);

        const char* book = cb_book_name(app->selected_book_index);

        char line1[64];
        snprintf(line1, sizeof(line1), "%s", book);

        char line2[32];
        snprintf(line2, sizeof(line2), "Ch %u : Verse %u",
                 (unsigned)app->selected_chapter, (unsigned)app->selected_verse);

        widget_add_string_element(app->widget, 4, 14, AlignLeft, AlignTop, FontPrimary, line1);
        widget_add_string_element(app->widget, 4, 34, AlignLeft, AlignTop, FontSecondary, line2);
        widget_add_string_element(app->widget, 4, 54, AlignLeft, AlignTop, FontSecondary,
                                  "Reader text next.");

        view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
        return true;
    }

    return false;
}

static void catholic_bible_scene_browse_verses_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
    widget_reset(app->widget);
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
    catholic_bible_scene_search_on_enter,
    catholic_bible_scene_about_on_enter,
};

static bool (*const catholic_bible_on_event_handlers[])(void*, SceneManagerEvent) = {
    catholic_bible_scene_menu_on_event,
    catholic_bible_scene_browse_books_on_event,
    catholic_bible_scene_browse_chapters_on_event,
    catholic_bible_scene_browse_verses_on_event,
    catholic_bible_scene_search_on_event,
    catholic_bible_scene_about_on_event,
};

static void (*const catholic_bible_on_exit_handlers[])(void*) = {
    catholic_bible_scene_menu_on_exit,
    catholic_bible_scene_browse_books_on_exit,
    catholic_bible_scene_browse_chapters_on_exit,
    catholic_bible_scene_browse_verses_on_exit,
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

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, catholic_bible_navigation_callback);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, catholic_bible_custom_event_callback);

    // Attach to GUI (fullscreen)
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Register views
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewWidget, widget_get_view(app->widget));

    // Start at menu
    scene_manager_next_scene(app->scene_manager, CatholicBibleSceneMenu);

    return app;
}

static void catholic_bible_app_free(CatholicBibleApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewWidget);

    submenu_free(app->submenu);
    widget_free(app->widget);

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
