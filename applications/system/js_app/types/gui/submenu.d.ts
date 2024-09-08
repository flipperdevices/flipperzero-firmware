import type { View } from ".";
import type { Contract } from "../event_loop";

/**
 * Menu with text entries
 */
export class Submenu implements View {
    _view: symbol;
    chosen: Contract<number>;
    setHeader(header: string): void;
    setItems(items: string[]): void;
}

export function make(): Submenu;
