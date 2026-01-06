#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>

/* ===== App Struct ===== */

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Submenu* submenu;
    Widget* widget;
} CatholicBibleApp;

/* ===== View IDs ===== */

typedef enum {
    CatholicBibleViewMenu = 0,
    CatholicBibleViewWidget,
} CatholicBibleViewId;

/* ===== Scene IDs ===== */

typedef enum {
    CatholicBibleSceneMenu = 0,
    CatholicBibleSceneBrowse,
    CatholicBibleSceneSearch,
    CatholicBibleSceneAbout,
    CatholicBibleSceneCount,
} CatholicBibleSceneId;

/* ===== Menu Events ===== */

typedef enum {
    CatholicBibleMenuEventBrowse = 1,
    CatholicBibleMenuEventSearch,
    CatholicBibleMenuEventAbout,
} CatholicBibleMenuEvent;

/* ===== Forward Declarations ===== */

static void scene_menu_on_enter(void* context);
static bool scene_menu_on_event(void* context, SceneManagerEvent event);
static void scene_menu_on_exit(void* context);

static void scene_browse_on_enter(void* context);
static bool scene_browse_on_event(void* context, SceneManagerEvent event);
static void scene_browse_on_exit(void* context);

static void scene_search_on_enter(void* context);
static bool scene_search_on_event(void* context, SceneManagerEvent event);
static void scene_search_on_exit(void* context);

static void scene_about_on_enter(void* context);
static bool scene_about_on_event(void* context, SceneManagerEvent event);
static void scene_about_on_exit(void* context);

/* ===== Scene Handler Tables (SDK 1.4.x compatible) ===== */

static void (*const on_enter_handlers[])(void*) = {
    scene_menu_on_enter,
    scene_browse_on_enter,
    scene_search_on_enter,
    scene_about_on_enter,
};

static bool (*const on_event_handlers[])(void*, SceneManagerEvent) = {
    scene_menu_on_event,
    scene_browse_on_event,
    scene_search_on_event,
    scene_about_on_event,
};

static void (*const on_exit_handlers[])(void*) = {
    scene_menu_on_exit,
    scene_browse_on_exit,
    scene_search_on_exit,
    scene_about_on_exit,
};

static const SceneManagerHandlers scene_manager_handlers = {
    .on_enter_handlers = on_enter_handlers,
    .on_event_handlers = on_event_handlers,
    .on_exit_handlers = on_exit_handlers,
    .scene_num = CatholicBibleSceneCount,
};

/* ===== Callbacks ===== */

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

/* ===== Scenes ===== */

static void scene_menu_on_enter(void* context) {
    CatholicBibleApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Catholic Bible");

    submenu_add_item(app->submenu, "Browse", CatholicBibleMenuEventBrowse, submenu_callback, app);
    submenu_add_item(app->submenu, "Search", CatholicBibleMenuEventSearch, submenu_callback, app);
    submenu_add_item(app->submenu, "About", CatholicBibleMenuEventAbout, submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewMenu);
}

static bool scene_menu_on_event(void* context, SceneManagerEvent event) {
    CatholicBibleApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == CatholicBibleMenuEventBrowse) {
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneBrowse);
            return true;
        } else if(event.event == CatholicBibleMenuEventSearch) {
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneSearch);
            return true;
        } else if(event.event == CatholicBibleMenuEventAbout) {
            scene_manager_next_scene(app->scene_manager, CatholicBibleSceneAbout);
            return true;
        }
    }

    return false;
}

static void scene_menu_on_exit(void* context) {
    CatholicBibleApp* app = context;
    submenu_reset(app->submenu);
}

static void scene_browse_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 5, 10, AlignLeft, AlignTop, FontPrimary, "Browse");
    widget_add_string_element(app->widget, 5, 28, AlignLeft, AlignTop, FontSecondary, "Book / Chapter / Verse");

    view_dispatcher_switch_to_view(app->view_dispatcher, CatholicBibleViewWidget);
}

static bool scene_browse_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static void scene_browse_on_exit(void* context) {
    CatholicBibleApp* app = context;
    widget_reset(app->widget);
}

static void scene_search_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 5, 10, AlignLeft, AlignTop, FontPrimary, "Search");
    widget_add_string_element(app->widget, 5, 28, AlignLeft, AlignTop, FontSecondary, "Indexed verse search");

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

static void scene_about_on_enter(void* context) {
    CatholicBibleApp* app = context;

    widget_reset(app->widget);
    widget_add_string_element(app->widget, 5, 10, AlignLeft, AlignTop, FontPrimary, "About");
    widget_add_string_element(app->widget, 5, 28, AlignLeft, AlignTop, FontSecondary, "Catholic Bible for Flipper");

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

/* ===== App Entry ===== */

int32_t catholic_bible_app(void* p) {
    UNUSED(p);

    CatholicBibleApp* app = malloc(sizeof(CatholicBibleApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&scene_manager_handlers, app);

    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, navigation_callback);

    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, CatholicBibleViewWidget, widget_get_view(app->widget));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, CatholicBibleSceneMenu);
    view_dispatcher_run(app->view_dispatcher);

    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewMenu);
    view_dispatcher_remove_view(app->view_dispatcher, CatholicBibleViewWidget);

    submenu_free(app->submenu);
    widget_free(app->widget);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    free(app);

    return 0;
}
