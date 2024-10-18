/**
 * @file GUI system
 * @version Added in JS SDK 0.1
 */

import type { Contract } from "../event_loop";

type Properties = { [K: string]: any };

export declare class View<Props extends Properties> {
    set<P extends keyof Props>(property: P, value: Props[P]): void;
}

export declare class ViewFactory<Props extends Properties, V extends View<Props>> {
    make(): V;
    makeWith(initial: Partial<Props>): V;
}

/**
 * @version Added in JS SDK 0.1
 */
declare class ViewDispatcher {
    /**
     * Event source for `sendCustom` events
     * @version Added in JS SDK 0.1
     */
    custom: Contract<number>;
    /**
     * Event source for navigation events (back key presses)
     * @version Added in JS SDK 0.1
     */
    navigation: Contract;
    /**
     * Sends a number to the custom event handler
     * @param event number to send
     * @version Added in JS SDK 0.1
     */
    sendCustom(event: number): void;
    /**
     * Switches to a view
     * @param assoc View-ViewDispatcher association as returned by `add`
     * @version Added in JS SDK 0.1
     */
    switchTo(assoc: View<any>): void;
    /**
     * Sends this ViewDispatcher to the front or back, above or below all other
     * GUI viewports
     * @param direction Either `"front"` or `"back"`
     * @version Added in JS SDK 0.1
     */
    sendTo(direction: "front" | "back"): void;
}

/**
 * @version Added in JS SDK 0.1
 */
export const viewDispatcher: ViewDispatcher;
