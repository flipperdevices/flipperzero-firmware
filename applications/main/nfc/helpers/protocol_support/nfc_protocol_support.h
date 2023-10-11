/**
 * @file nfc_protocol_support.h
 * @brief Protocol support for NFC application.
 *
 * NFC protocol support helper abstracts common scenes with a single interface
 * and lets each protocol decide on concrete implementation.
 *
 * @see nfc_protocol_support_base.h
 * @see nfc_protocol_support_common.h
 */
#pragma once

#include <gui/scene_manager.h>

#include "nfc_protocol_support_common.h"

/**
 * @brief Abstract interface for on_enter() scene handler.
 *
 * Is to be called whenever a scene is entered to.
 *
 * @param[in] scene identifier of the scene associated with the handler.
 * @param[in,out] context pointer to a user-specified context (will be passed to concrete handler).
 */
void nfc_protocol_support_on_enter(NfcProtocolSupportScene scene, void* context);

/**
 * @brief Abstract interface for on_event() scene handler.
 *
 * @param[in] scene identifier of the scene associated with the handler.
 * @param[in,out] context pointer to a user-specified context (will be passed to concrete handler).
 * @param[in] event SceneManager event to be handled by the scene.
 * @returns true if the event was consumed, false otherwise.
 */
bool nfc_protocol_support_on_event(
    NfcProtocolSupportScene scene,
    void* context,
    SceneManagerEvent event);

/**
 * @brief Abstract interface for on_exit() scene handler.
 *
 * Is to be called whenever a scene is exited from.
 *
 * @param[in] scene identifier of the scene associated with the handler.
 * @param[in,out] context pointer to a user-specified context (will be passed to concrete handler).
 */
void nfc_protocol_support_on_exit(NfcProtocolSupportScene scene, void* context);
