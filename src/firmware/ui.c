#include "ui.h"
#include "log.h"
#include "version.h"

void ui_header() {
    char buffer[1024];

    log_string("\n\r");
    log_string("\n\r");
    log_string("\n\r");

    snprintf(buffer, sizeof(buffer), "%s %s\n\r", APPLICATION_NAME, APPLICATION_VERSION);
    log_string(buffer);

    snprintf(buffer, sizeof(buffer), "Build at %s\n\r", APPLICATION_BUILD);
    log_string(buffer);

#ifdef APPLICATION_GIT_SHA1
    snprintf(buffer, sizeof(buffer), "Git: %s\n\r", APPLICATION_GIT_SHA1);
    log_string(buffer);
#endif

    log_string("\n\r");
    log_string("\n\r");
}
