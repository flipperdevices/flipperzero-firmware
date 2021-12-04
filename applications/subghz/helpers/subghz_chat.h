#pragma once
#include "../subghz_i.h"

typedef struct SubGhzChatWorker SubGhzChatWorker;

typedef enum {
    SubghzChatEventNoEvent,
    SubghzChatEventUserEntrance,
    SubghzChatEventUserExit,
    SubghzChatEventInputData,
    SubghzChatEventRXData,
} SubghzChatEvent;

typedef struct {
    SubghzChatEvent event;
    char c;
} SubghzChatQueue;

SubGhzChatWorker* subghz_chat_worker_alloc();
void subghz_chat_worker_free(SubGhzChatWorker* instance);
bool subghz_chat_worker_start(SubGhzChatWorker* instance, uint32_t frequency);
void subghz_chat_worker_stop(SubGhzChatWorker* instance);
bool subghz_chat_worker_is_running(SubGhzChatWorker* instance);
SubghzChatQueue subghz_chat_worker_get_event_chat(SubGhzChatWorker* instance);
size_t subghz_chat_worker_available(SubGhzChatWorker* instance);
size_t subghz_chat_worker_read(SubGhzChatWorker* instance, uint8_t* data, size_t size);
bool subghz_chat_worker_write(SubGhzChatWorker* instance, uint8_t* data, size_t size);
