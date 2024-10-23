/**
 * Unit test module. Only available if the firmware has been configured with
 * `FIRMWARE_APP_SET=unit_tests`.
 * @version Added in JS SDK 0.1
 * @module
 */

export function fail(message: string): never;
export function assert_eq<T>(expected: T, result: T): void | never;
export function assert_float_close(expected: number, result: number, epsilon: number): void | never;
