#include <dialogs/dialogs.h>

#include "../minunit.h"

MU_TEST(test_dialog_file_browser_set_basic_options_should_init_all_fields) {
    mu_assert(
        sizeof(DialogsFileBrowserOptions) == 28,
        "Changes to `DialogsFileBrowserOptions` should also be reflected in `dialog_file_browser_set_basic_options`");

    DialogsFileBrowserOptions options;
    dialog_file_browser_set_basic_options(&options, ".fap", NULL);
    // note: this assertions can safely be changed, their primary purpose is to remind the maintainer
    // to update `dialog_file_browser_set_basic_options` by including all structure fields in it
    mu_assert_string_eq(".fap", options->extension);
    mu_assert_null(options->base_path);
    mu_assert(options->skip_assets, "`skip_assets` should default to `true");
    mu_assert(options->hide_dot_files, "`hide_dot_files` should default to `true");
    mu_assert_null(options->icon);
    mu_assert(options->hide_ext, "`hide_ext` should default to `true");
    mu_assert_null(options->item_loader_callback);
    mu_assert_null(options->item_loader_context);
}

// Disabled: due to field alignment some bytes remain unaffected by `dialog_file_browser_set_basic_options`
// which makes the assertion fail.
//
//MU_TEST(test_dialog_file_browser_set_basic_options_should_init_all_fields) {
//    DialogsFileBrowserOptions options;
//    // fill the structures with 'n's
//    memset(&options, 'n', sizeof(DialogsFileBrowserOptions));
//    // default-initialize the structure (omitting any 'n's in the parameters)
//    dialog_file_browser_set_basic_options(&options, ".fap", NULL);
//    // assert that no bytes remained set to 'n' after default-initialiaztion
//    mu_assert(
//        memchr(&options, 'n', sizeof(DialogsFileBrowserOptions)) == NULL,
//        "`options` contained an 'n' symbol, although it should have been overwritten");
//}

MU_TEST_SUITE(dialogs_file_browser_options) {
    MU_RUN_TEST(test_dialog_file_browser_set_basic_options_should_init_all_fields);
}

int run_minunit_test_dialogs_file_browser_options() {
    MU_RUN_SUITE(dialogs_file_browser_options);

    return MU_EXIT_CODE;
}
