/**
 * @file concurrent_runner.h
 * @brief Helper for running code in a separate thread.
 *
 * Sometimes it is necessary to quickly spawn a thread, run a blocking
 * operation (such as loading a file or processing a large chunk of data)
 * and delete it upon completion.
 *
 * The ConcurrentRunner helper automates this task by taking a function
 * and running it in a separate thread. Another optional function
 * can be provided to notify the caller about completion of the task.
 *
 * Input and output parameters are handled through the context parameter.
 * ConcurrentRunner does not provide any error handling of the user-supplied
 * code, so it must be taken care of on the application side.
 *
 * @warning The application code MUST make sure that all relevant pointers
 * (i.e. callbacks and the context) remain valid until the full completion
 * of the task being run.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback type declaration for use with ConcurrentRunner.
 *
 * @param[in,out] context Pointer to a user-specific object.
 */
typedef void (*ConcurrentRunnerCallback)(void* context);

/**
 * @brief Execute a function in a separate thread.
 *
 * If completion notification is not necessary, pass NULL to finished_callback.
 *
 * @warning Setting stack_size too low will lead to a system crash.
 *
 * @param[in] stack_size size of the created thread, in bytes.
 * @param[in] run_callback pointer to a function to be run in a separate thread.
 * @param[in] finished_callback optional pointer to a function to be called upon completion of the task.
 * @param[in,out] context pointer to a user-specific object. Will be passed to both callbacks.
 */
void concurrent_runner_start(
    uint32_t stack_size,
    ConcurrentRunnerCallback run_callback,
    ConcurrentRunnerCallback finished_callback,
    void* context);

#ifdef __cplusplus
}
#endif
