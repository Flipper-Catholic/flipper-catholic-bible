static bool catholic_bible_navigation_callback(void* context) {
    CatholicBibleApp* app = context;
    view_dispatcher_stop(app->view_dispatcher);
    return true;
}
static bool catholic_bible_navigation_callback(void* context) {
    CatholicBibleApp* app = context;
    view_dispatcher_stop(app->view_dispatcher);
    return true;
}#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>

/*
 * Minimal Catholic Bible app skeleton.
 * This version only displays a placeholder screen and exits cleanly.
 */

typedef struct {
    ViewDispatcher* view_dispatcher;
    Widget* widget;
} CatholicBibleApp;

static void catholic_bible_exit_callback(void* context) {
    CatholicBibleApp* app = context;
    view_dispatcher_stop(app->view_dispatcher);
}

int32_t catholic_bible_app(void* p) {
    UNUSED(p);

    CatholicBibleApp* app = malloc(sizeof(CatholicBibleApp));

    app->view_dispatcher = view_dispatcher_alloc();
    app->widget = widget_alloc();

    widget_add_string_element(
        app->widget,
        64,
        32,
        AlignCenter,
        AlignCenter,
        FontPrimary,
        "Catholic Bible"
    );

    widget_add_string_element(
        app->widget,
        64,
        46,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        "Coming soon"
    );

    view_dispatcher_add_view(
        app->view_dispatcher,
        0,
        widget_get_view(app->widget)
    );

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
        app->view_dispatcher,
    catholic_bible_exit_callback
    );
    view_dispatcher_set_navigation_event_callback(
    app->view_dispatcher,
    catholic_bible_navigation_callback
    );
    view_dispatcher_set_exit_callback(
    app->view_dispatcher,
    catholic_bible_exit_callback
    );

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        app->view_dispatcher,
        gui,
        ViewDispatcherTypeFullscreen
    );

    view_dispatcher_switch_to_view(app->view_dispatcher, 0);
    view_dispatcher_run(app->view_dispatcher);

    furi_record_close(RECORD_GUI);

    view_dispatcher_remove_view(app->view_dispatcher, 0);
    widget_free(app->widget);
    view_dispatcher_free(app->view_dispatcher);
    free(app);

    return 0;
}
