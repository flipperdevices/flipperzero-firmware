#include "passport.h"

void passport_alloc(Passport* passport) {
    furi_assert(passport);

    // Load Passport Settings
    if(!(passport_settings_load(&passport->settings))) {
        passport->settings.background = BG_MARIO;
        passport->settings.image = PIMG_GOKUSET;
        passport->settings.name = true;
        passport->settings.mood_set = MOOD_SET_420;
        passport->settings.level = true;
        passport->settings.xp_text = true;
        passport->settings.xp_mode = XP_MODE_BAR_PERCENT;
        passport->settings.multipage = true;
        passport_settings_save(&passport->settings);
    }

    // Load Desktop Settings
    DESKTOP_SETTINGS_LOAD(&passport->desktop_settings);

    //animations
    if(passport->settings.multipage) {
        animations[AniRedVirus] = icon_animation_alloc(&A_DrMario_red_virus_25x25);
        animations[AniYelVirus] = icon_animation_alloc(&A_DrMario_yellow_virus_25x25);
        animations[AniBluVirus] = icon_animation_alloc(&A_DrMario_blue_virus_25x25);

        icon_animation_start(animations[AniRedVirus]);
        icon_animation_start(animations[AniYelVirus]);
        icon_animation_start(animations[AniBluVirus]);
    }

    //stats, xp, levels
    passport->dolphin = furi_record_open(RECORD_DOLPHIN);
    passport->stats = dolphin_stats(passport->dolphin);
    furi_record_close(RECORD_DOLPHIN);

    DolphinStats* stats = &passport->stats;

    passport->max_level = dolphin_state_max_level();

    //get XP
    passport->xp_to_levelup = dolphin_state_xp_to_levelup(stats->icounter);
    passport->xp_above_last_levelup = dolphin_state_xp_above_last_levelup(stats->icounter);
    passport->xp_for_current_level = passport->xp_to_levelup + passport->xp_above_last_levelup;

    //get percentage from remaining XP
    if(stats->level == passport->max_level) {
        passport->xp_progress = 100;
    } else {
        passport->xp_progress =
            round(((passport->xp_above_last_levelup * 100) / passport->xp_for_current_level));
    }

    //mood
    passport->mood = 0; // DONT NEED DIFFERENT PICS BASED ON MOOD
    passport->moodStrIndex = stats->butthurt;

    // set mood to "Happy" if dumbmode is enabled
    if(passport->desktop_settings.is_dumbmode && passport->settings.image != PIMG_DOLPHINMOODY)
        passport->moodStrIndex = passport->moodStrIndex + 4;

    // portrait
    passport->tmpLvl = 0;
    furi_assert((stats->level > 0) && (stats->level <= passport->max_level));
    if(passport->settings.image == PIMG_GOKUSET) {
        passport->tmpLvl = 3;
        if(stats->level > 10) passport->tmpLvl = 4;
        if(stats->level > 15) passport->tmpLvl = 4;
        if(stats->level > 18) passport->tmpLvl = 5;
        if(stats->level > 21) passport->tmpLvl = 5;
        if(stats->level > 24) passport->tmpLvl = 6;
        if(stats->level > 27) passport->tmpLvl = 6;
    } else if(passport->settings.image == PIMG_DOLPHIN) {
        passport->tmpLvl = 0;
        if(stats->level > 10) passport->tmpLvl = 1;
        if(stats->level > 20) passport->tmpLvl = 2;
    } else if(passport->settings.image == PIMG_DOLPHINMOODY) {
        passport->tmpLvl = 0;
        if(stats->level > 10) passport->tmpLvl = 1;
        if(stats->level > 20) passport->tmpLvl = 2;
        passport->tmpMood = 2;
        if(stats->butthurt <= 9) passport->tmpMood = 1;
        if(stats->butthurt <= 4) passport->tmpMood = 0;
    }

    //start animation for sonic passport image if selected
    if(passport->settings.image == PIMG_RABBIT) {
        animations[AniRabbit] = icon_animation_alloc(&A_Rabbit_46x49);

        icon_animation_start(animations[AniRabbit]);
    }

    if(passport->settings.image == PIMG_SLIME) {
        animations[AniSlime] = icon_animation_alloc(&A_Slime_46x49);

        icon_animation_start(animations[AniSlime]);
    }

    if(passport->settings.image == PIMG_SONIC) {
        animations[AniSonic] = icon_animation_alloc(&A_Sonic_46x49);

        icon_animation_start(animations[AniSonic]);
    }

    //string variables set
    //name
    if(furi_hal_version_get_name_ptr()) {
        snprintf(passport->my_name, 9, "%s", furi_hal_version_get_name_ptr());
    } else {
        snprintf(passport->my_name, 9, "%s", "Unknown");
    }
    //mood
    if(passport->settings.mood_set != 0) {
        snprintf(
            passport->mood_str,
            20,
            "Mood: %s",
            moods[passport->settings.mood_set][passport->moodStrIndex]);
    }
    //level
    snprintf(passport->level_str, 12, "Level: %hu", stats->level);

    if(passport->settings.multipage) {
        snprintf(passport->level_str1, 12, "%hu", stats->level);
    }

    //determine whether to display XP stats or max level
    if(stats->level == passport->max_level) {
        //xp display for main passport view
        snprintf(passport->xp_str, 14, "Max Level");

        if(passport->settings.multipage) {
            //xp display for Dr. Mario view
            snprintf(passport->xp_str1, 7, "Max");
            snprintf(passport->xp_str2, 7, "Level");
        }
    } else {
        //xp display for main passport view
        snprintf(
            passport->xp_str,
            14,
            "%lu/%lu",
            passport->xp_above_last_levelup,
            passport->xp_for_current_level);

        if(passport->settings.multipage) {
            //xp display for Dr. Mario view
            snprintf(passport->xp_str1, 7, "%lu", passport->xp_above_last_levelup);
            snprintf(passport->xp_str2, 7, "%lu", passport->xp_for_current_level);
        }
    }

    //set first page shown
    passport->page = Main;

    free(stats);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    Passport* passport = ctx;
    furi_mutex_acquire(passport->mutex, FuriWaitForever);

    if(passport->page == Main) {
        //calc bar fill ratio
        //default bar is 65px wide
        passport->xp_fill = (int)round((passport->xp_progress / 100) * 65);

        // draw background
        switch(passport->settings.background) {
        case BG_NONE:
            break;
        case BG_DB:
            canvas_draw_icon(canvas, 0, 0, &I_passport_dragonball);
            break;
        case BG_CIRCUIT:
            canvas_draw_icon(canvas, 0, 0, &I_passport_Circuit_v1);
            break;
        case BG_STOCK:
            canvas_draw_icon(canvas, 0, 0, &I_passport_FlipperClassic);
            break;
        case BG_FURI:
            canvas_draw_icon(canvas, 0, 0, &I_passport_Furipass);
            break;
        case BG_MARIO:
            canvas_draw_icon(canvas, 0, 0, &I_passport_mario);
            break;
        case BG_MOUNTAINS:
            canvas_draw_icon(canvas, 0, 0, &I_passport_Mountains);
            break;
        case BG_MULTI:
            canvas_draw_icon(canvas, 0, 0, &I_passport_Multipass);
            break;
        case BG_SCROLL:
            canvas_draw_icon(canvas, 0, 0, &I_passport_Scroll);
            break;
        case BG_SLUT:
            canvas_draw_icon(canvas, 0, 0, &I_passport_SlutPass);
            break;
        }

        // draw portrait
        switch(passport->settings.image) {
        case PIMG_NONE:
            break;
        case PIMG_BRIAREOS:
            canvas_draw_icon(canvas, 11, 2, &I_Briareos_Hecatonchires);
            break;
        case PIMG_COBRA:
            canvas_draw_icon(canvas, 11, 2, &I_Cobra);
            break;
        case PIMG_DALI:
            canvas_draw_icon(canvas, 11, 2, &I_Dali_Mask);
            break;
        case PIMG_DEER:
            canvas_draw_icon(canvas, 11, 2, &I_Deer);
            break;
        case PIMG_DOLPHIN:
            canvas_draw_icon(canvas, 11, 2, portrait[passport->tmpLvl]);
            break;
        case PIMG_DOLPHINMOODY:
            canvas_draw_icon(canvas, 11, 2, portraitsMoody[passport->tmpMood][passport->tmpLvl]);
            break;
        case PIMG_ED209:
            canvas_draw_icon(canvas, 11, 2, &I_ED209);
            break;
        case PIMG_FSOCIETY:
            canvas_draw_icon(canvas, 11, 2, &I_FSociety_Mask);
            break;
        case PIMG_FSOCIETY2:
            canvas_draw_icon(canvas, 11, 2, &I_fsoc_pass);
            break;
        case PIMG_GOKUSET:
            canvas_draw_icon(canvas, 11, 2, portrait[passport->tmpLvl]);
            break;
        case PIMG_GOKUKID:
            canvas_draw_icon(canvas, 11, 2, &I_G0ku);
            break;
        case PIMG_GOKUADULT:
            canvas_draw_icon(canvas, 11, 2, &I_g0ku_1);
            break;
        case PIMG_GOKUSSJ:
            canvas_draw_icon(canvas, 11, 2, &I_g0ku_2);
            break;
        case PIMG_GOKUSSJ3:
            canvas_draw_icon(canvas, 11, 2, &I_g0ku_3);
            break;
        case PIMG_GUYFAWKES:
            canvas_draw_icon(canvas, 11, 2, &I_Guy_Fawkes_Mask);
            break;
        case PIMG_LAIN:
            canvas_draw_icon(canvas, 11, 2, &I_Lain);
            break;
        case PIMG_MARIO:
            canvas_draw_icon(canvas, 11, 2, &I_Pixel_Mario);
            break;
        case PIMG_MARVIN:
            canvas_draw_icon(canvas, 11, 2, &I_Marvin);
            break;
        case PIMG_MORELEELLOO:
            canvas_draw_icon(canvas, 11, 2, &I_Moreleeloo);
            break;
        case PIMG_NEUROMANCER:
            canvas_draw_icon(canvas, 11, 2, &I_Neuromancer);
            break;
        case PIMG_PIKASLEEPY:
            canvas_draw_icon(canvas, 11, 2, &I_PikaSleepy);
            break;
        case PIMG_RABBIT:
            canvas_draw_icon_animation(canvas, 11, 2, animations[AniRabbit]);
            break;
        case PIMG_SHINKAI:
            canvas_draw_icon(canvas, 11, 2, &I_Shinkai);
            break;
        case PIMG_SLIME:
            canvas_draw_icon_animation(canvas, 11, 2, animations[AniSlime]);
            break;
        case PIMG_SONIC:
            canvas_draw_icon_animation(canvas, 11, 2, animations[AniSonic]);
            break;
        case PIMG_SPIDER:
            canvas_draw_icon(canvas, 11, 2, &I_Spider_Jerusalem);
            break;
        case PIMG_TANKGIRL:
            canvas_draw_icon(canvas, 11, 2, &I_Tank_Girl);
            break;
        case PIMG_TOTORO:
            canvas_draw_icon(canvas, 11, 2, &I_Totoro);
            break;
        case PIMG_WAIFU1:
            canvas_draw_icon(canvas, 11, 2, &I_Waifu1);
            break;
        case PIMG_WAIFU2:
            canvas_draw_icon(canvas, 11, 2, &I_Waifu2);
            break;
        case PIMG_WAIFU3:
            canvas_draw_icon(canvas, 11, 2, &I_Waifu3);
            break;
        }

        //draw flipper info
        canvas_set_font(canvas, FontSecondary);
        //name
        if(passport->settings.name) {
            canvas_draw_str(canvas, 58, 10, passport->my_name);
        }
        //mood
        if(passport->settings.mood_set != 0) {
            canvas_draw_str(canvas, 58, 22, passport->mood_str);
        }
        //level
        if(passport->settings.level) {
            canvas_draw_str(canvas, 58, 34, passport->level_str);
        }

        canvas_set_font(canvas, FontBatteryPercent);

        //xp stats
        if(passport->settings.xp_text) {
            canvas_draw_str(canvas, 58, 42, passport->xp_str);
        }

        canvas_set_font(canvas, FontSecondary);

        //blank out background
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 58, 44, 67, 7);
        canvas_set_color(canvas, ColorBlack);

        char percent[5];

        switch(passport->settings.xp_mode) {
        case 0:
            //Bar
            //draw black xp bar which will be used as outline but also for full bar
            canvas_draw_rbox(canvas, 58, 45, 67, 6, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 46, 65, 4, 1);

            //fill bar according to xp percentage
            if(passport->xp_fill >= 2) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_rbox(canvas, 59, 46, passport->xp_fill, 4, 1);
            }
            break;
        case 1:
            //Percent
            //draw black xp bar which will be used as outline but also for full bar
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_rbox(canvas, 58, 44, 67, 8, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 45, 65, 6, 1);

            //fill bar fully
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_rbox(canvas, 59, 45, 65, 6, 1);

            //put percentage text
            canvas_set_font(canvas, FontBatteryPercent);
            canvas_set_color(canvas, ColorWhite);

            snprintf(percent, 5, "%d%%", (int)passport->xp_progress);
            canvas_draw_str_aligned(canvas, 90, 45, AlignCenter, AlignTop, percent);

            canvas_set_color(canvas, ColorBlack);
            canvas_set_font(canvas, FontSecondary);
            break;
        case 2:
            //Inverted Percent
            //draw black xp bar which will be used as outline but also for full bar
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_rbox(canvas, 58, 44, 67, 8, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 45, 65, 6, 1);

            //put percentage text
            canvas_set_font(canvas, FontBatteryPercent);
            canvas_set_color(canvas, ColorBlack);

            snprintf(percent, 5, "%d%%", (int)passport->xp_progress);
            canvas_draw_str_aligned(canvas, 90, 45, AlignCenter, AlignTop, percent);

            canvas_set_font(canvas, FontSecondary);
            break;
        case 3:
            //Retro 3
            //draw black xp bar which will be used as outline but also for full bar
            canvas_draw_rbox(canvas, 58, 45, 67, 6, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 46, 65, 4, 1);

            //create retro x3 look - outline
            canvas_set_color(canvas, ColorBlack);
            //first section
            //no start - drawing end curve
            canvas_draw_dot(canvas, 77, 46);
            canvas_draw_dot(canvas, 78, 46);
            canvas_draw_dot(canvas, 78, 47);
            canvas_draw_dot(canvas, 78, 48);
            canvas_draw_dot(canvas, 78, 49);
            canvas_draw_dot(canvas, 77, 49);
            //second section
            //start curve
            canvas_draw_dot(canvas, 81, 46);
            canvas_draw_dot(canvas, 80, 46);
            canvas_draw_dot(canvas, 80, 47);
            canvas_draw_dot(canvas, 80, 48);
            canvas_draw_dot(canvas, 80, 49);
            canvas_draw_dot(canvas, 81, 49);
            //end curve
            canvas_draw_dot(canvas, 99, 46);
            canvas_draw_dot(canvas, 100, 46);
            canvas_draw_dot(canvas, 100, 47);
            canvas_draw_dot(canvas, 100, 48);
            canvas_draw_dot(canvas, 100, 49);
            canvas_draw_dot(canvas, 99, 49);
            //third section
            //start curve (no end curve)
            canvas_draw_dot(canvas, 103, 46);
            canvas_draw_dot(canvas, 102, 46);
            canvas_draw_dot(canvas, 102, 47);
            canvas_draw_dot(canvas, 102, 48);
            canvas_draw_dot(canvas, 102, 49);
            canvas_draw_dot(canvas, 103, 49);

            //fill bar according to xp percentage
            if(passport->xp_fill >= 2) {
                canvas_draw_rbox(canvas, 59, 46, passport->xp_fill, 4, 1);
            }

            //create retro x3 look - white parts after fill
            canvas_set_color(canvas, ColorWhite);
            //between first and second sections
            canvas_draw_dot(canvas, 78, 45);
            canvas_draw_dot(canvas, 79, 45);
            canvas_draw_dot(canvas, 80, 45);

            canvas_draw_dot(canvas, 79, 46);
            canvas_draw_dot(canvas, 79, 47);
            canvas_draw_dot(canvas, 79, 48);
            canvas_draw_dot(canvas, 79, 49);

            canvas_draw_dot(canvas, 78, 50);
            canvas_draw_dot(canvas, 79, 50);
            canvas_draw_dot(canvas, 80, 50);
            //between second and third sections
            canvas_draw_dot(canvas, 100, 45);
            canvas_draw_dot(canvas, 101, 45);
            canvas_draw_dot(canvas, 102, 45);

            canvas_draw_dot(canvas, 101, 46);
            canvas_draw_dot(canvas, 101, 47);
            canvas_draw_dot(canvas, 101, 48);
            canvas_draw_dot(canvas, 101, 49);

            canvas_draw_dot(canvas, 100, 50);
            canvas_draw_dot(canvas, 101, 50);
            canvas_draw_dot(canvas, 102, 50);
            break;
        case 4:
            //Retro 5
            //draw black xp bar which will be used as outline but also for full bar
            canvas_draw_rbox(canvas, 58, 45, 67, 6, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 46, 65, 4, 1);

            //create retro x5 look - outline
            canvas_set_color(canvas, ColorBlack);
            //first section
            //no start - drawing end curve
            canvas_draw_dot(canvas, 71, 46);
            canvas_draw_dot(canvas, 72, 46);
            canvas_draw_dot(canvas, 72, 47);
            canvas_draw_dot(canvas, 72, 48);
            canvas_draw_dot(canvas, 72, 49);
            canvas_draw_dot(canvas, 71, 49);
            //second section
            //start curve
            canvas_draw_dot(canvas, 75, 46);
            canvas_draw_dot(canvas, 74, 46);
            canvas_draw_dot(canvas, 74, 47);
            canvas_draw_dot(canvas, 74, 48);
            canvas_draw_dot(canvas, 74, 49);
            canvas_draw_dot(canvas, 75, 49);
            //end curve
            canvas_draw_dot(canvas, 83, 46);
            canvas_draw_dot(canvas, 84, 46);
            canvas_draw_dot(canvas, 84, 47);
            canvas_draw_dot(canvas, 84, 48);
            canvas_draw_dot(canvas, 84, 49);
            canvas_draw_dot(canvas, 83, 49);
            //third section
            //start curve
            canvas_draw_dot(canvas, 87, 46);
            canvas_draw_dot(canvas, 86, 46);
            canvas_draw_dot(canvas, 86, 47);
            canvas_draw_dot(canvas, 86, 48);
            canvas_draw_dot(canvas, 86, 49);
            canvas_draw_dot(canvas, 87, 49);
            //end curve
            canvas_draw_dot(canvas, 95, 46);
            canvas_draw_dot(canvas, 96, 46);
            canvas_draw_dot(canvas, 96, 47);
            canvas_draw_dot(canvas, 96, 48);
            canvas_draw_dot(canvas, 96, 49);
            canvas_draw_dot(canvas, 95, 49);
            //fourth section
            //start curve
            canvas_draw_dot(canvas, 99, 46);
            canvas_draw_dot(canvas, 98, 46);
            canvas_draw_dot(canvas, 98, 47);
            canvas_draw_dot(canvas, 98, 48);
            canvas_draw_dot(canvas, 98, 49);
            canvas_draw_dot(canvas, 99, 49);
            //end curve
            canvas_draw_dot(canvas, 107, 46);
            canvas_draw_dot(canvas, 108, 46);
            canvas_draw_dot(canvas, 108, 47);
            canvas_draw_dot(canvas, 108, 48);
            canvas_draw_dot(canvas, 108, 49);
            canvas_draw_dot(canvas, 107, 49);
            //fifth section
            //start curve (no end curve)
            canvas_draw_dot(canvas, 111, 46);
            canvas_draw_dot(canvas, 110, 46);
            canvas_draw_dot(canvas, 110, 47);
            canvas_draw_dot(canvas, 110, 48);
            canvas_draw_dot(canvas, 110, 49);
            canvas_draw_dot(canvas, 111, 49);

            //fill bar according to xp percentage
            if(passport->xp_fill >= 2) {
                canvas_draw_rbox(canvas, 59, 46, passport->xp_fill, 4, 1);
            }

            //create retro x3 look - white parts after fill
            canvas_set_color(canvas, ColorWhite);
            //between first and second sections
            canvas_draw_dot(canvas, 72, 45);
            canvas_draw_dot(canvas, 73, 45);
            canvas_draw_dot(canvas, 74, 45);

            canvas_draw_dot(canvas, 73, 46);
            canvas_draw_dot(canvas, 73, 47);
            canvas_draw_dot(canvas, 73, 48);
            canvas_draw_dot(canvas, 73, 49);

            canvas_draw_dot(canvas, 72, 50);
            canvas_draw_dot(canvas, 73, 50);
            canvas_draw_dot(canvas, 74, 50);
            //between second and third sections
            canvas_draw_dot(canvas, 84, 45);
            canvas_draw_dot(canvas, 85, 45);
            canvas_draw_dot(canvas, 86, 45);

            canvas_draw_dot(canvas, 85, 46);
            canvas_draw_dot(canvas, 85, 47);
            canvas_draw_dot(canvas, 85, 48);
            canvas_draw_dot(canvas, 85, 49);

            canvas_draw_dot(canvas, 84, 50);
            canvas_draw_dot(canvas, 85, 50);
            canvas_draw_dot(canvas, 86, 50);
            //between third and fourth sections
            canvas_draw_dot(canvas, 96, 45);
            canvas_draw_dot(canvas, 97, 45);
            canvas_draw_dot(canvas, 98, 45);

            canvas_draw_dot(canvas, 97, 46);
            canvas_draw_dot(canvas, 97, 47);
            canvas_draw_dot(canvas, 97, 48);
            canvas_draw_dot(canvas, 97, 49);

            canvas_draw_dot(canvas, 96, 50);
            canvas_draw_dot(canvas, 97, 50);
            canvas_draw_dot(canvas, 98, 50);
            //between fourth and fifth sections
            canvas_draw_dot(canvas, 108, 45);
            canvas_draw_dot(canvas, 109, 45);
            canvas_draw_dot(canvas, 110, 45);

            canvas_draw_dot(canvas, 109, 46);
            canvas_draw_dot(canvas, 109, 47);
            canvas_draw_dot(canvas, 109, 48);
            canvas_draw_dot(canvas, 109, 49);

            canvas_draw_dot(canvas, 108, 50);
            canvas_draw_dot(canvas, 109, 50);
            canvas_draw_dot(canvas, 110, 50);
            break;
        case 5:
            //Percent with Bar
            //draw black xp bar which will be used as outline but also for full bar
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_rbox(canvas, 58, 44, 67, 8, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 45, 65, 6, 1);

            //fill bar according to xp percentage
            if(passport->xp_fill >= 2) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_rbox(canvas, 59, 45, passport->xp_fill, 6, 1);
            }

            //put percentage text
            canvas_set_font(canvas, FontBatteryPercent);

            if(((int)passport->xp_progress >= 0) && ((int)passport->xp_progress <= 40)) {
                canvas_set_color(canvas, ColorBlack);
                snprintf(percent, 5, "%d%%", (int)passport->xp_progress);
                canvas_draw_str_aligned(canvas, 90, 45, AlignCenter, AlignTop, percent);
            } else if(((int)passport->xp_progress > 40) && ((int)passport->xp_progress <= 60)) {
                canvas_set_color(canvas, ColorBlack);
                snprintf(percent, 5, "%d%%", (int)passport->xp_progress);
                canvas_draw_str_aligned(canvas, 112, 45, AlignCenter, AlignTop, percent);
            } else {
                canvas_set_color(canvas, ColorWhite);
                snprintf(percent, 5, "%d%%", (int)passport->xp_progress);
                canvas_draw_str_aligned(canvas, 90, 45, AlignCenter, AlignTop, percent);
                canvas_set_color(canvas, ColorBlack);
            }
            canvas_set_color(canvas, ColorBlack);
            canvas_set_font(canvas, FontSecondary);
            break;
        case 6:
            //None

            break;
        default:
            //draw black xp bar which will be used as outline but also for full bar
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_rbox(canvas, 58, 45, 67, 6, 1);

            //hollow out xp bar with white "empty" space
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_rbox(canvas, 59, 46, 65, 4, 1);

            //fill bar according to xp percentage
            if(passport->xp_fill >= 2) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_rbox(canvas, 59, 46, passport->xp_fill, 4, 1);
            }
            break;
        }
    } else if(passport->page == DrMario) {
        //calc bar fill ratio
        //dr mario pill jar fill area is 50px tall
        passport->xp_fill = (int)round((passport->xp_progress / 100) * 50);

        //backdrop
        canvas_draw_icon(canvas, 0, 0, &I_DrMario_backdrop_128x64);

        //fill in stats
        canvas_set_font(canvas, FontSecondary);

        // level
        canvas_draw_str(canvas, 89, 50, "Level:");
        canvas_draw_str(canvas, 110, 58, passport->level_str1);
        // xp
        canvas_draw_str(canvas, 40, 44, "XP:");

        canvas_set_font(canvas, FontBatteryPercent);

        if(strcmp(passport->xp_str1, "Max") == 0) {
            canvas_draw_str_aligned(canvas, 60, 45, AlignCenter, AlignTop, passport->xp_str1);
            canvas_draw_str_aligned(canvas, 60, 54, AlignCenter, AlignTop, passport->xp_str2);
        } else {
            canvas_draw_str_aligned(canvas, 60, 45, AlignCenter, AlignTop, passport->xp_str1);
            canvas_draw_line(canvas, 45, 52, 75, 52);
            canvas_draw_str_aligned(canvas, 60, 54, AlignCenter, AlignTop, passport->xp_str2);
        }

        canvas_set_font(canvas, FontSecondary);

        //fill bar black
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_box(canvas, 5, 9, 28, 50);

        //use white bar to fill based on xp_fill value
        if(passport->xp_fill < 50) {
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_box(canvas, 5, 9, 28, 50 - passport->xp_fill);
        }

        canvas_set_color(canvas, ColorBlack);

        //draw sprites
        canvas_draw_icon_animation(canvas, 41, 3, animations[AniRedVirus]);
        canvas_draw_icon_animation(canvas, 70, 3, animations[AniYelVirus]);
        canvas_draw_icon_animation(canvas, 99, 3, animations[AniBluVirus]);
    }
    furi_mutex_release(passport->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PassportEvent event = {.type = EventKeyPress, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

void passport_free(Passport* passport) {
    furi_assert(passport);

    //stop and clear animations
    if(passport->settings.multipage) {
        icon_animation_stop(animations[AniRedVirus]);
        icon_animation_free(animations[AniRedVirus]);
        icon_animation_stop(animations[AniYelVirus]);
        icon_animation_free(animations[AniYelVirus]);
        icon_animation_stop(animations[AniBluVirus]);
        icon_animation_free(animations[AniBluVirus]);
    }

    if(passport->settings.image == PIMG_RABBIT) {
        icon_animation_free(animations[AniRabbit]);
    }

    if(passport->settings.image == PIMG_SONIC) {
        icon_animation_free(animations[AniSonic]);
    }
    furi_mutex_free(passport->mutex);
    free(passport);
}

int32_t passport_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PassportEvent));

    Passport* passport = malloc(sizeof(Passport));
    passport->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!passport->mutex) {
        FURI_LOG_E("Passport", "cannot create mutex\r\n");
        passport_free(passport);
        return 255;
    }

    passport_alloc(passport);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, passport);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PassportEvent event = {0};

    while(event.input.key != InputKeyBack) {
        if(furi_message_queue_get(event_queue, &event, 10) != FuriStatusOk) {
            continue;
        }
        furi_mutex_acquire(passport->mutex, FuriWaitForever);
        if(event.type == EventKeyPress) {
            switch(event.input.key) {
            case InputKeyUp:
                break;
            case InputKeyDown:
                break;
            case InputKeyRight:
                if(passport->settings.multipage) {
                    switch(passport->page) {
                    case Main:
                        passport->page = DrMario;
                        break;
                    case DrMario:
                        passport->page = Main;
                        break;
                    default:
                        break;
                    }
                }
                break;
            case InputKeyLeft:
                if(passport->settings.multipage) {
                    switch(passport->page) {
                    case Main:
                        passport->page = DrMario;
                        break;
                    case DrMario:
                        passport->page = Main;
                        break;
                    default:
                        break;
                    }
                }
                break;
            case InputKeyOk:
                break;
            default:
                break;
            }
        }
        view_port_update(view_port);
        furi_mutex_release(passport->mutex);
    }
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    gui = NULL;
    furi_message_queue_free(event_queue);
    passport_free(passport);
    return 0;
}