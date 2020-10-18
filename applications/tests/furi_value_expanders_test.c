#include "flipper_v2.h"
#include "minunit.h"
#include <stdint.h>

static const uint32_t notify_value_0 = 0x12345678;
static const uint32_t notify_value_1 = 0x11223344;

static uint32_t pubsub_value = 0;

void test_value_manager_handler(void* arg, void* ctx) {
    pubsub_value = *(uint32_t*)arg;
}

void test_furi_value_manager() {
    uint32_t value = 0;
    ValueManager managed;

    mu_check(init_managed(&managed, &value, sizeof(value)));

    pubsub_value = 0;

    PubSubItem* test_pubsub_item;
    test_pubsub_item = subscribe_pubsub(&managed.pubsub, test_value_manager_handler, 0);
    mu_assert_pointers_not_eq(test_pubsub_item, NULL);

    mu_check(write_managed(&managed, (void*)&notify_value_0, sizeof(notify_value_0), 100));

    mu_assert_int_eq(pubsub_value, notify_value_0);

    uint32_t* ptr = acquire_mutex(&managed.value, 100);
    mu_assert_pointers_not_eq(ptr, NULL);

    *ptr = notify_value_1;

    mu_check(commit_managed(&managed, ptr));

    mu_assert_int_eq(pubsub_value, notify_value_1);
}
