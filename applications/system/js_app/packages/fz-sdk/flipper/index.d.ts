/**
 * Module for querying device properties
 * @version Added in JS SDK 0.1
 * @module
 */

/**
 * @brief Returns the device model
 * @version Added in JS SDK 0.1
 */
export declare function getModel(): string;

/**
 * @brief Returns the name of the virtual dolphin
 * @version Added in JS SDK 0.1
 */
export declare function getName(): string;

/**
 * @brief Returns the battery charge percentage
 * @version Added in JS SDK 0.1
 */
export declare function getBatteryCharge(): number;

/**
 * @warning Do **NOT** use this to check the presence or absence of features. If
 *          you do, I'm gonna be sad :( Instead, refer to `checkSdkFeatures` and
 *          other similar mechanisms.
 * @note Original firmware reports `"flipperdevices"`.
 * @version Added in JS SDK 0.1
 */
export declare const firmwareVendor: string;

/**
 * @warning Do **NOT** use this to check the presence or absence of features. If
 *          you do, I'm gonna be sad :( Instead, refer to
 *          `checkSdkCompatibility` and other similar mechanisms.
 * @note You're looking at JS SDK 0.1
 * @version Added in JS SDK 0.1
 */
export declare const jsSdkVersion: [number, number];
