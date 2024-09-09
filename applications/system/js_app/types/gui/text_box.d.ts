import type { View } from ".";
import type { Contract } from "../event_loop";

/**
 * Simple text box
 */
export class TextBox implements View {
    _view: symbol;
    input: Contract<string>;
    setText(text: string): void;
}

/**
 * Creates a text box
 */
export function make(font: "text" | "hex", focus: "start" | "end"): TextBox;
