#include "mine_sweeper_haptic.h"
#include "../minesweeper.h"

void mine_sweeper_play_happy_bump(void* context) {
    MineSweeperApp* app = context;

    notification_message(app->notification, &sequence_set_vibro_on);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 20);
    notification_message(app->notification, &sequence_reset_vibro);
}

void mine_sweeper_play_long_ok_bump(void* context) {
    MineSweeperApp* app = context;

    for(int i = 0; i < 2; i++) {
        notification_message(app->notification, &sequence_set_vibro_on);
        furi_thread_flags_wait(0, FuriFlagWaitAny, 20);
        notification_message(app->notification, &sequence_reset_vibro);
        furi_thread_flags_wait(0, FuriFlagWaitAny, 20);
    }
}

void mine_sweeper_play_oob_bump(void* context) {
    MineSweeperApp* app = context;

    notification_message(app->notification, &sequence_set_vibro_on);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 20);
    notification_message(app->notification, &sequence_reset_vibro);
}

void mine_sweeper_play_lose_bump(void* context) {
    MineSweeperApp* app = context;

    notification_message(app->notification, &sequence_set_vibro_on);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 100);
    notification_message(app->notification, &sequence_reset_vibro);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 400);
}

void mine_sweeper_play_win_bump(void* context) {
    MineSweeperApp* app = context;

    for(int i = 0; i < 4; i++) {
        notification_message(app->notification, &sequence_set_vibro_on);
        furi_thread_flags_wait(0, FuriFlagWaitAny, 50);
        notification_message(app->notification, &sequence_reset_vibro);
        furi_thread_flags_wait(0, FuriFlagWaitAny, 100);
    }
}
