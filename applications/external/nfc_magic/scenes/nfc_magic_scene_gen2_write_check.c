#include "../nfc_magic_app_i.h"

void nfc_magic_scene_gen2_write_check_view_callback(WriteProblemsEvent event, void* context) {
    NfcMagicApp* instance = context;
    NfcMagicAppWriteProblemsContext* problems_context = &instance->write_problems_context;

    if(event == WriteProblemsEventCenterPressed) {
        if(problems_context->problem_index == problems_context->problems_total - 1) {
            // Continue to the next scene
            if(instance->gen2_poller_is_wipe_mode) {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWipe);
            } else {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWrite);
            }
        } else {
            // Move to the next problem
            problems_context->problem_index++;
            problems_context->problem_index_abs++;
            write_problems_set_problem_index(
                instance->write_problems, problems_context->problem_index);

            for(uint8_t i = problems_context->problem_index_abs;
                i < GEN2_POLLER_WRITE_PROBLEMS_LEN;
                i++) {
                if(problems_context->problems.all_problems & (1 << i)) {
                    write_problems_set_content(instance->write_problems, gen2_problem_strings[i]);
                    problems_context->problem_index_abs = i;
                    break;
                }
            }
        }
    } else if(event == WriteProblemsEventLeftPressed) {
        if(problems_context->problem_index == 0) {
            // Exit to the previous scene
            scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, NfcMagicSceneMfClassicMenu);
        } else {
            // Move to the previous problem
            problems_context->problem_index--;
            problems_context->problem_index_abs--;
            write_problems_set_problem_index(
                instance->write_problems, problems_context->problem_index);

            for(uint8_t i = problems_context->problem_index_abs;
                i < GEN2_POLLER_WRITE_PROBLEMS_LEN;
                i--) {
                if(problems_context->problems.all_problems & (1 << i)) {
                    write_problems_set_content(instance->write_problems, gen2_problem_strings[i]);
                    problems_context->problem_index_abs = i;
                    break;
                }
            }
        }
    }
}

void nfc_magic_scene_gen2_write_check_on_enter(void* context) {
    NfcMagicApp* instance = context;

    Gen2PollerWriteProblems problems = gen2_poller_check_target_problems(instance->target_dev);
    if(!instance->gen2_poller_is_wipe_mode) {
        problems.all_problems |=
            gen2_poller_check_source_problems(instance->source_dev).all_problems;
    }

    WriteProblems* write_problems = instance->write_problems;
    uint8_t problems_count = 0;
    uint8_t current_problem = 0;
    furi_assert(!problems.no_data, "No MFC data in nfc device");

    if(problems.all_problems == 0) {
        if(instance->gen2_poller_is_wipe_mode) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWipe);
            return;
        } else {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWrite);
            return;
        }
    }

    // Count the total number of problems
    for(uint8_t i = 0; i < GEN2_POLLER_WRITE_PROBLEMS_LEN; i++) {
        if(problems.all_problems & (1 << i)) {
            problems_count++;
        }
    }

    // Init the view
    write_problems_set_callback(
        write_problems, nfc_magic_scene_gen2_write_check_view_callback, instance);
    write_problems_set_problems_total(write_problems, problems_count);
    write_problems_set_problem_index(write_problems, current_problem);

    // Set the initial content to the first problem
    for(uint8_t i = 0; i < GEN2_POLLER_WRITE_PROBLEMS_LEN; i++) {
        if(problems.all_problems & (1 << i)) {
            write_problems_set_content(write_problems, gen2_problem_strings[i]);
            current_problem = i;
            break;
        }
    }

    // Save the context
    instance->write_problems_context.problem_index = current_problem;
    instance->write_problems_context.problems_total = problems_count;
    instance->write_problems_context.problems = problems;

    // Setup and start worker
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewWriteProblems);
}

bool nfc_magic_scene_gen2_write_check_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    UNUSED(event);
    UNUSED(context);
    UNUSED(instance);
    bool consumed = false;

    return consumed;
}

void nfc_magic_scene_gen2_write_check_on_exit(void* context) {
    NfcMagicApp* instance = context;

    instance->write_problems_context.problem_index = 0;
    instance->write_problems_context.problems_total = 0;
    instance->write_problems_context.problems.all_problems = 0;

    write_problems_reset(instance->write_problems);
}
