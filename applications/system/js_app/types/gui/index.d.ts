import type { Contract } from "../event_loop";

type Lit = number | string | null | undefined | {};

export interface View {
    _view: symbol;
}

/**
 * Opaque association between a View and a ViewDispatcher
 */
export type Association = number;

declare class ViewDispatcher {
    /**
     * Sends a number to the custom event handler
     * @param event number to send
     */
    sendCustom(event: number): void;
    /**
     * Returns a contract that can be used to subscribe to an event in an event loop
     * @param event Event type: `"custom"` or `"navigation"`
     */
    event<T extends "custom" | "navigation">(event: T): Contract<T extends "custom" ? number : undefined>;
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
