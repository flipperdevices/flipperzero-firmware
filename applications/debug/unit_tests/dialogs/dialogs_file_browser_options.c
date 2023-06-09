#include <dialogs/dialogs.h>

#include "../minunit.h"

MU_TEST(test_dialog_file_browser_set_basic_options_should_init_all_fields) {
    DialogsFileBrowserOptions options;
    // fill the structures with 'n's
    memset(&options, 'n', sizeof(DialogsFileBrowserOptions));
    // default-initialize the structure (omitting any 'n's in the parameters)
    dialog_file_browser_set_basic_options(&options, ".fap", NULL);
    // assert that no bytes remained set to 'n' after default-initialiaztion
    mu_assert(
        memchr(&options, 'n', sizeof(DialogsFileBrowserOptions)) == NULL,
        "`options` contained an 'n' symbol although it should have been overwritten");
}

MU_TEST_SUITE(dialogs_file_browser_options) {
    MU_RUN_TEST(test_dialog_file_browser_set_basic_options_should_init_all_fields);
}

int run_minunit_test_dialogs_file_browser_options() {
    MU_RUN_SUITE(dialogs_file_browser_options);

    return MU_EXIT_CODE;
}
