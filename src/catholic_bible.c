#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>

#include "books_meta.h"

/* ===================== App Struct ===================== */

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Submenu* submenu;
    Widget* widget;

    uint32_t selected_book;
    uint32_t selected_chapter;
} CatholicBibleApp;

/* ===================== Views ===================== */

typedef enum {
    CatholicBibleViewMenu = 0,
    CatholicBibleViewSubmenu,
    CatholicBibleViewWidget,
} CatholicBibleViewId;

/* ===================== Scenes ===================== */

typedef enum {
    CatholicBibleSceneMenu = 0,
    CatholicBibleSceneBooks,    // Browse -> list of books
    CatholicBibleSceneChapters, // Book -> list of chapters (real counts)
    CatholicBibleSceneChapter,  // Chapter -> placeholder screen
    CatholicBibleSceneSearch,
    CatholicBibleSceneAbout,
    CatholicBibleSceneCount,
} CatholicBibleSceneId;

/* ===================== Menu Events ===================== */

typedef enum {
    MenuEventBrowse = 1,
    MenuEventSearch,
    MenuEventAbout,
} MenuEvent;

/* ===================== Catholic Book List ===================== */

static const char* catholic_books[] = {
    "Genesis", "Exodus", "Leviticus", "Numbers", "Deuteronomy",
    "Joshua", "Judges", "Ruth",
    "1 Samuel", "2 Samuel", "1 Kings", "2 Kings",
    "1 Chronicles", "2 Chronicles",
    "Ezra", "Nehemiah", "Tobit", "Judith", "Esther",
    "1 Maccabees", "2 Maccabees",
    "Job", "Psalms", "Proverbs", "Ecclesiastes",
    "Song of Songs", "Wisdom", "Sirach",
    "Isaiah", "Jeremiah", "Lamentations", "Baruch",
    "Ezekiel", "Daniel",
    "Hosea", "Joel", "Amos", "Obadiah", "Jonah", "Micah",
    "Nahum", "Habakkuk", "Zephaniah", "Haggai", "Zechariah", "Malachi",
    "Matthew", "Mark", "Luke", "John", "Acts",
    "Romans", "1 Corinthians", "2 Corinthians", "Galatians",
    "Ephesians", "Philippians", "Colossians",
    "1 Thessalonians", "2 Thessalonians",
    "1 Timothy", "2 Timothy", "Titus", "Philemon",
    "Hebrews", "James", "1 Peter", "2 Peter",
    "1 John", "2 John", "3 John", "Jude",
    "Revelation",
};

static const uint32_t catholic_books_count =
    sizeof(catholic_books) / sizeof(catholic_books[0]);

/* ===================== Helpers ===================== */

static uint16_t get_chapter_count(uint32_t book_index) {
    if(book_index >= catholic_books_count) return 1;

    const uint32_t counts_len =
        sizeof(catholic_book_chapter_counts) / sizeof(catholic_book_chapter_counts[0]);

    if(book_index >= counts_len) return 1;

    uint16_t c = catholic_book_chapter_counts[book_index];
    return (c == 0) ? 1 : c;
}

/* ===================== Forward Declarations ===================== */

static void scene_menu_on_enter(void* context);
static bool scene_menu_on_event(void* context, SceneManagerEvent event);
static void scene_menu_on_exit(void* context);

static void scene_books_on_enter(void* context);
static bool scene_books_on_event(void* context, SceneManagerEvent event);
static void scene_books_on_exit(void* context);

static void scene_chapters_on_enter(void* context);
static bool scene_chapters_on_event(void* context, SceneManagerEvent event);
static void scene_chapters_on_exit(void* context);

static void scene_chapter_on_enter(void* context);
static bool scene_chapter_on_event(void* context, SceneManagerEvent event);
static void scene_chapter_on_exit(void* context);

static void scene_search_on_enter(void* context);
static bool scene_search_on_event(void* context, SceneManagerEvent event);
static void scene_search_on_exit(void* context);

static void scene_about_on_enter(void* context);
static bool scene_about_on_event(void* context, SceneManagerEvent event);
static void scene_about_on_exit(void* context);

/* ===================== Scene Tables ===================== */

static void (*const on_enter_handlers[])(void*) = {
    scene_menu_on_enter,
    scene_books_on_enter,
    scene_chapters_on_enter,
    scene_chapter_on_enter,
    scene_search_on_enter,
    scene_about_on_enter,
};

static bool (*const on_event_handlers[])(void*, SceneManagerEvent) = {
    scene_menu_on_event,
    scene_books_on_event,
    scene_chapters_on_event,
    scene_chapter_on_event,
    scene_search_on_event,
    scene_about_on_event,
};

static void (*const on_exit_handlers[])(void*) = {
    scene_menu_on_exit,
    scene_books_on_exit,
    scene_chapters_on_exit,
    scene_chapter_on_exit,
    scene_search_on_exit,
    scene_about_on_exit,
};

static const SceneManagerHandlers scene_handlers = {
    .on_enter_handlers = on_enter_handlers,
    .on_event_handlers = on_event_handlers,
    .on_exit_handlers = on_exit_handlers,
    .scene_num = CatholicBibleSceneCount,
};

/* ===================== Callbacks ===================== */

static void submenu_callback(void* context, uint32_t index) {
    CatholicBibleApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static bool navigation_callback(void* context) {
    CatholicBibleApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static bool custom_event_callback(void* context, uint32_t event) {
    CatholicBibleApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

/* ===================== Scenes ===================== */

static void scene_menu_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Catholic Bible");

    submenu_add_item(app->submenu, "Browse", MenuEventBrowse, submenu_callback, app);
    submenu_add_item(app->submenu, "Search", MenuEventSearch, submenu_callback, app);
    submenu_add_item(app->submenu, "About", MenuEventAbout, submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewMenu);
}

static bool scene_menu_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MenuEventBrowse) {
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBooks);
            return true;
        }
        if(event.event == MenuEventSearch) {
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneSearch);
            return true;
        }
        if(event.event == MenuEventAbout) {
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneAbout);
            return true;
        }
    }
    return false;
}

static void scene_menu_on_exit(void* context) {
    UNUSED(context);
}

/* ---- Books (Browse) ---- */

static void scene_books_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Books");

    for(uint32_t i = 0; i < catholic_books_count; i++) {
        submenu_add_item(app->submenu, catholic_books[i], i, submenu_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool scene_books_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        app->selected_book = event.event;
        app->selected_chapter = 1;
        scene_manager_next_scene(app->scene_manager, CatholicBibleSceneChapters);
        return true;
    }
    return false;
}

static void scene_books_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

/* ---- Chapters (Real counts) ---- */

static void scene_chapters_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);

    if(app->selected_book >= catholic_books_count) app->selected_book = 0;
    const char* book_name = catholic_books[app->selected_book];
    submenu_set_header(app->submenu, book_name);

    uint16_t chapter_count = get_chapter_count(app->selected_book);

    char label[24];
    for(uint32_t ch = 1; ch <= chapter_count; ch++) {
        snprintf(label, sizeof(label), "Chapter %lu", (unsigned long)ch);
        submenu_add_item(app->submenu, label, ch, submenu_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewSubmenu);
}

static bool scene_chapters_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        uint32_t ch = event.event;
        if(ch < 1) ch = 1;
        app->selected_chapter = ch;
        scene_manager_next_scene(app->scene_manager, CatholicBibleSceneChapter);
        return true;
    }
    return false;
}

static void scene_chapters_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

/* ---- Chapter Screen (Still placeholder verses) ---- */

static void scene_chapter_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);

    if(app->selected_book >= catholic_books_count) app->selected_book = 0;
    const char* book_name = catholic_books[app->selected_book];

    widget_add_string_element(app->widget, 5, 10, AlignLeft, AlignTop, FontPrimary, book_name);

    char line[24];
    snprintf(line, sizeof(line), "Chapter %lu", (unsigned long)app->selected_chapter);
    widget_add_string_element(app->widget, 5, 28, AlignLeft, AlignTop, FontSecondary, line);

    widget_add_string_element(
        app->widget, 5, 44, AlignLeft, AlignTop, FontSecondary, "Verses coming next");

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
}

static bool scene_chapter_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static void scene_chapter_on_exit(void* context) {
    CatholicBibleApp* app = context;
    widget_reset(app->widget);
}

/* ---- Search ---- */

static void scene_search_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 5, 10, AlignLeft, AlignTop, FontPrimary, "Search");
    widget_add_string_element(app->widget, 5, 28, AlignLeft, AlignTop, FontSecondary, "Index TBD");

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
}

static bool scene_search_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static void scene_search_on_exit(void* context) {
    CatholicBibleApp* app = context;
    widget_reset(app->widget);
}

/* ---- About ---- */

static void scene_about_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 5, 10, AlignLeft, AlignTop, FontPrimary, "About");
    widget_add_string_element(app->widget, 5, 28, AlignLeft, AlignTop, FontSecondary, "Catholic Bible (Flipper)");

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
}

static bool scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static void scene_about_on_exit(void* context) {
    CatholicBibleApp* app = context;
    widget_reset(app->widget);
}

/* ===================== App Entry ===================== */

int32_t catholic_bible_app(void* p) {
    UNUSED(p);

    CatholicBibleApp* app = malloc(sizeof(CatholicBibleApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&scene_handlers, app);

    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    app->selected_book = 0;
    app->selected_chapter = 1;

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, navigation_callback);

    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewWidget, widget_get_view(app->widget));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, CatholicBibleSceneMenu);
    view_dispatcher_run(app->view_dispatcher);

    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewMenu);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewWidget);

    submenu_free(app->submenu);
    widget_free(app->widget);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    free(app);

    return 0;
}
