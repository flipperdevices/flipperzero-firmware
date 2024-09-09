import type { Contract } from "../event_loop";

export interface View {
    _view: symbol;
}

/**
 * Opaque association between a View and a ViewDispatcher
 */
export type Association = number;

declare class ViewDispatcher {
    /**
     * Event source for `sendCustom` events
     */
    custom: Contract<number>;
    /**
     * Event source for navigation events (back key presses)
     */
    navigation: Contract;
    /**
     * Sends a number to the custom event handler
     * @param event number to send
     */
    sendCustom(event: number): void;
    /**
     * Associates a View with a ViewDispatcher
     * @param view View to add
     * @returns Association identifier that can be used with `remove` and `switch_to`
     */
    add(view: View): Association;
    /**
     * Removes a view
     * @param assoc View-ViewDispatcher association as returned by `add`
     */
    remove(assoc: Association): void;
    /**
     * Switches to a view
     * @param assoc View-ViewDispatcher association as returned by `add`
     */
    switchTo(assoc: Association): void;
    /**
     * Sends this ViewDispatcher to the front or back, above or below all other
     * GUI viewports
     * @param direction Either `"front"` or `"back"`
     */
    sendTo(direction: "front" | "back"): void;
}

export const viewDispatcher: ViewDispatcher;
