#include <stdio.h>
#include <string.h>
#include "flipper_v2.h"
#include "log.h"

#include "minunit.h"

const int context_value = 0xdeadbeef;
const int notify_value = 0x12345678;

int pubsub_value = 0;
int pubsub_context_value = 0;

void test_pubsub_handler(void* arg, void* ctx) {
    pubsub_value = *(int*)arg;
    pubsub_context_value = *(int*)ctx;
}

void test_furi_pubsub() {
    bool result;
    PubSub test_pubsub;
    PubSubItem* test_pubsub_item;

    result = init_pubsub(&test_pubsub);
    mu_assert(result, "init pubsub failed");

    test_pubsub_item = subscribe_pubsub(&test_pubsub, test_pubsub_handler, (void*)&context_value);
    mu_assert_pointers_not_eq(test_pubsub_item, NULL);

    notify_pubsub(&test_pubsub, (void*)&notify_value);
    mu_assert_int_eq(pubsub_value, notify_value);
    mu_assert_int_eq(pubsub_context_value, context_value);

    
}