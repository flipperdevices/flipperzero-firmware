#ifndef EXTRA_H_
#define EXTRA_H_

/*
 * Initializes EXTRA.
 */
void extra_init();

/*
 * Registers EXTRA commands.
 */
void extra_register();

/*
 * @brief Same as extra_init() and extra_register().
 * Initializes EXTRA and registers EXTRA commands.
 */
void extra_init_register();

/*
 * Deinitializes EXTRA
 */
void extra_deinit();

/*
 * Unregisters EXTRA commands
 */
void extra_unregister();

/*
 * @brief Same as extra_deinit() and extra_unregister().
 * Deinitializes EXTRA and unregisters EXTRA commands.
 */
void extra_deinit_unregister();

/*
 * Checks if EXTRA commands are currently registered.
 * @return bool
 */
bool extra_registered();

/*
 * Checks if EXTRA is currently initialized.
 * @return bool
 */
bool extra_initialized();

#endif
