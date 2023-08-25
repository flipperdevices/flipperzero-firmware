#include "../subbrute_i.h"
#include "subbrute_scene.h"

#define TAG "SubBruteSceneLoadFile"

#define MIN_TD 0
#define MAX_TD 255
#define MIN_REP 1
#define MAX_REP 100
#define MIN_TE 100
#define MAX_TE 600

enum SubBruteVarListIndex {
    SubBruteVarListIndexTimeDelay,
    SubBruteVarListIndexRepeat_or_OnExtra,
    SubBruteVarListIndexTe,
};

static void setup_extra_enter_callback(void* context, uint32_t index);

static void setup_extra_td_callback(VariableItem* item) {
    furi_assert(item);
    SubBruteState* instance = variable_item_get_context(item);
    furi_assert(instance);
    char buf[6];

    const uint8_t index = variable_item_get_current_value_index(item);
    uint8_t val = subbrute_worker_get_timeout(instance->worker);

    if(index == 0) {
        if(val > MIN_TD) {
            val--;
            subbrute_worker_set_timeout(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MIN_TD) {
                variable_item_set_current_value_index(item, 0);
            }
        }
    } else if(index == 2) {
        if(val < MAX_TD) {
            val++;
            subbrute_worker_set_timeout(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MAX_TD) {
                variable_item_set_current_value_index(item, 2);
            }
        }
    } else if(index == 1) {
        if(val == MIN_TD) {
            val++;
            subbrute_worker_set_timeout(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MAX_TD) {
                variable_item_set_current_value_index(item, 2);
            }
        } else if(val == MAX_TD) {
            val--;
            subbrute_worker_set_timeout(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MIN_TD) {
                variable_item_set_current_value_index(item, 0);
            }
        }
    }
}

static void setup_extra_rep_callback(VariableItem* item) {
    furi_assert(item);
    SubBruteState* instance = variable_item_get_context(item);
    furi_assert(instance);
    char buf[6];

    const uint8_t index = variable_item_get_current_value_index(item);
    uint8_t val = subbrute_worker_get_repeats(instance->worker);

    if(index == 0) {
        if(val > MIN_REP) {
            val--;
            subbrute_worker_set_repeats(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MIN_REP) {
                variable_item_set_current_value_index(item, 0);
            }
        }
    } else if(index == 2) {
        if(val < MAX_REP) {
            val++;
            subbrute_worker_set_repeats(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MAX_REP) {
                variable_item_set_current_value_index(item, 2);
            }
        }
    } else if(index == 1) {
        if(val == MIN_REP) {
            val++;
            subbrute_worker_set_repeats(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MAX_REP) {
                variable_item_set_current_value_index(item, 2);
            }
        } else if(val == MAX_REP) {
            val--;
            subbrute_worker_set_repeats(instance->worker, val);
            snprintf(&buf[0], 5, "%d", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MIN_REP) {
                variable_item_set_current_value_index(item, 0);
            }
        }
    }
}

static void setup_extra_te_callback(VariableItem* item) {
    furi_assert(item);
    SubBruteState* instance = variable_item_get_context(item);
    furi_assert(instance);
    char buf[6];

    const uint8_t index = variable_item_get_current_value_index(item);
    uint32_t val = subbrute_worker_get_te(instance->worker);

    if(index == 0) {
        if(val > MIN_TE) {
            val--;
            subbrute_worker_set_te(instance->worker, val);
            snprintf(&buf[0], 5, "%ld", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MIN_TE) {
                variable_item_set_current_value_index(item, 0);
            }
        }
    } else if(index == 2) {
        if(val < MAX_TE) {
            val++;
            subbrute_worker_set_te(instance->worker, val);
            snprintf(&buf[0], 5, "%ld", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MAX_TE) {
                variable_item_set_current_value_index(item, 2);
            }
        }
    } else if(index == 1) {
        if(val == MIN_TE) {
            val++;
            subbrute_worker_set_te(instance->worker, val);
            snprintf(&buf[0], 5, "%ld", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MAX_TE) {
                variable_item_set_current_value_index(item, 2);
            }
        } else if(val == MAX_TE) {
            val--;
            subbrute_worker_set_te(instance->worker, val);
            snprintf(&buf[0], 5, "%ld", val);
            variable_item_set_current_value_text(item, &buf[0]);
            variable_item_set_current_value_index(item, 1);
            if(val == MIN_TE) {
                variable_item_set_current_value_index(item, 0);
            }
        }
    }
}

static void subbrute_scene_setup_extra_init_var_list(SubBruteState* instance, bool on_extra) {
    furi_assert(instance);
    char str[6];
    VariableItem* item;
    static bool extra = false;
    if(on_extra) {
        extra = true;
    }

    VariableItemList* var_list = instance->var_list;

    variable_item_list_reset(var_list);

    item = variable_item_list_add(var_list, "TimeDelay", 3, setup_extra_td_callback, instance);
    snprintf(&str[0], 5, "%d", subbrute_worker_get_timeout(instance->worker));
    variable_item_set_current_value_text(item, &str[0]);
    switch(subbrute_worker_get_timeout(instance->worker)) {
    case MIN_TD:
        variable_item_set_current_value_index(item, 0);
        break;
    case MAX_TD:
        variable_item_set_current_value_index(item, 2);
        break;

    default:
        variable_item_set_current_value_index(item, 1);
        break;
    }

    if(extra) {
        item = variable_item_list_add(var_list, "Repeats", 3, setup_extra_rep_callback, instance);
        snprintf(&str[0], 5, "%d", subbrute_worker_get_repeats(instance->worker));
        variable_item_set_current_value_text(item, &str[0]);
        switch(subbrute_worker_get_repeats(instance->worker)) {
        case MIN_REP:
            variable_item_set_current_value_index(item, 0);
            break;
        case MAX_REP:
            variable_item_set_current_value_index(item, 2);
            break;

        default:
            variable_item_set_current_value_index(item, 1);
            break;
        }
        const uint32_t te = subbrute_worker_get_te(instance->worker);
        if(te != 0) {
            item = variable_item_list_add(var_list, "Te", 3, setup_extra_te_callback, instance);
            snprintf(&str[0], 5, "%ld", te);
            variable_item_set_current_value_text(item, &str[0]);
            switch(te) {
            case MIN_TE:
                variable_item_set_current_value_index(item, 0);
                break;
            case MAX_TE:
                variable_item_set_current_value_index(item, 2);
                break;

            default:
                variable_item_set_current_value_index(item, 1);
                break;
            }
        }
    } else {
        item = variable_item_list_add(var_list, "Show Extra", 0, NULL, NULL);
    }

    variable_item_list_set_enter_callback(var_list, setup_extra_enter_callback, instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, SubBruteViewVarList);
}

static void setup_extra_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    SubBruteState* instance = context;

    if(index == SubBruteVarListIndexRepeat_or_OnExtra) {
        subbrute_scene_setup_extra_init_var_list(instance, true);
    }
}

void subbrute_scene_setup_extra_on_enter(void* context) {
    furi_assert(context);
    SubBruteState* instance = context;

    subbrute_scene_setup_extra_init_var_list(instance, false);
}

void subbrute_scene_setup_extra_on_exit(void* context) {
    furi_assert(context);
    SubBruteState* instance = context;

    variable_item_list_reset(instance->var_list);
}

bool subbrute_scene_setup_extra_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}