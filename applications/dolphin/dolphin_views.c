#include "dolphin_views.h"
#include <gui/view.h>
#include <gui/gui.h>
#include <gui/elements.h>

void dolphin_view_first_start_draw(Canvas* canvas, void* model) {
    DolphinViewFirstStartModel* m = model;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    if(m->page == 0) {
        canvas_draw_icon_name(canvas, 0, GUI_MAIN_HEIGHT - 53, I_DolphinFirstStart0_70x53);
        elements_multiline_text(canvas, 75, 20, "Hey m8,\npress > to\ncontinue");
    } else if(m->page == 1) {
        canvas_draw_icon_name(canvas, 0, GUI_MAIN_HEIGHT - 53, I_DolphinFirstStart1_59x53);
        elements_multiline_text(canvas, 64, 20, "First Of All,\n...      >");
    } else if(m->page == 2) {
        canvas_draw_icon_name(canvas, 0, GUI_MAIN_HEIGHT - 51, I_DolphinFirstStart2_59x51);
        elements_multiline_text(canvas, 64, 20, "Thank you\nfor your\nsupport! >");
    } else if(m->page == 3) {
        canvas_draw_icon_name(
            canvas, GUI_MAIN_WIDTH - 57, GUI_MAIN_HEIGHT - 48, I_DolphinFirstStart3_57x48);
        elements_multiline_text(canvas, 5, 20, "Kickstarter\ncampaign\nwas INSANE! >");
    } else if(m->page == 4) {
        canvas_draw_icon_name(
            canvas, GUI_MAIN_WIDTH - 67, GUI_MAIN_HEIGHT - 53, I_DolphinFirstStart4_67x53);
        elements_multiline_text(canvas, 5, 20, "Now\nallow me\nto introduce\nmyself >");
    } else if(m->page == 5) {
        canvas_draw_icon_name(canvas, 0, GUI_MAIN_HEIGHT - 53, I_DolphinFirstStart5_45x53);
        elements_multiline_text(
            canvas, 50, 20, "I am Flipper,\ncyberdolphin\nliving in your\npocket >");
    } else if(m->page == 6) {
        canvas_draw_icon_name(canvas, 0, GUI_MAIN_HEIGHT - 54, I_DolphinFirstStart6_58x54);
        elements_multiline_text(
            canvas, 63, 20, "I can grow\n smart'n'cool\nif you use me\noften >");
    } else if(m->page == 7) {
        canvas_draw_icon_name(
            canvas, GUI_MAIN_WIDTH - 61, GUI_MAIN_HEIGHT - 51, I_DolphinFirstStart7_61x51);
        elements_multiline_text(canvas, 5, 20, "As long as\nyou read, write\nand emulate >");
    } else if(m->page == 8) {
        canvas_draw_icon_name(
            canvas, GUI_MAIN_WIDTH - 56, GUI_MAIN_HEIGHT - 51, I_DolphinFirstStart8_56x51);
        elements_multiline_text(
            canvas, 5, 20, "You can check\nmy level and\nmood in the\nPassport menu >");
    }
}

void dolphin_view_idle_main_draw(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon_name(
        canvas,
        GUI_MAIN_WIDTH - 80,
        GUI_MAIN_HEIGHT - 60 + 6,
        I_Flipper_young_80x60);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "/\\: Stats");
    canvas_draw_str(canvas, 5, 32, "OK: Menu");
    canvas_draw_str(canvas, 2, 52, "\\/: Version");
}

void dolphin_view_idle_stats_draw(Canvas* canvas, void* model) {
    DolphinViewIdleStatsModel* m = model;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Dolphin stats:");

    char buffer[64];
    canvas_set_font(canvas, FontSecondary);
    snprintf(buffer, 64, "Icounter: %ld", m->icounter);
    canvas_draw_str(canvas, 5, 22, buffer);
    snprintf(buffer, 64, "Butthurt: %ld", m->butthurt);
    canvas_draw_str(canvas, 5, 32, buffer);
    canvas_draw_str(canvas, 5, 40, "< > change icounter");
}

void dolphin_view_idle_debug_draw(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Version info:");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 22, TARGET " " BUILD_DATE);
    canvas_draw_str(canvas, 5, 32, GIT_BRANCH);
    canvas_draw_str(canvas, 5, 42, GIT_BRANCH_NUM);
    canvas_draw_str(canvas, 5, 52, GIT_COMMIT);
}

uint32_t dolphin_view_idle_back(void* context) {
    return DolphinViewIdleMain;
}
