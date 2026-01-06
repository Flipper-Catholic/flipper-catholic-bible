#include <furi.h>

int32_t catholic_bible_app(void* p) {
    UNUSED(p);

    FuriString* msg = furi_string_alloc();
    furi_string_set(msg, "Catholic Bible\n(Coming Soon)");

    FuriLogLevel level = FuriLogLevelInfo;
    FURI_LOG(level, "%s", furi_string_get_cstr(msg));

    furi_string_free(msg);
    return 0;
}
