#include <furi.h>
#include <flipper_application/flipper_application.h>

#define APP_BASE_ARGS "run_in_background"

typedef enum {
    BGLoaderMessageType_AppReattached,
    BGLoaderMessageType_LoaderBackground,
    BGLoaderMessageType_LoaderExit,
} BGLoaderMessageType;

typedef struct {
    BGLoaderMessageType type;
} BGLoaderMessage;

typedef struct {
    FlipperApplication* fap;
    FuriThread* thread;
    FuriMessageQueue* to_app;
    FuriMessageQueue* to_loader;
} BGLoaderApp;
