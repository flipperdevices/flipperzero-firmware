import type { View } from ".";
import type { Contract } from "../event_loop";

/**
 * Menu with text entries
 */
export class Submenu implements View {
    _view: symbol;
    chosen: Contract<number>;
    /**
     * Sets the optional header
     * @param header 
     */
    setHeader(header: string): void;
    /**
     * Resets the header, item list and adds items from the supplied array
     * @param items list of menu items
     */
    setItems(items: string[]): void;
}

export function make(): Submenu;
