import type { View } from ".";
import type { Contract } from "../event_loop";

/**
 * Simple keyboard-like text input
 */
export class TextInput implements View {
    _view: symbol;
    input: Contract<string>;
}

/**
 * Creates a text input with a minimum and a maximum text length
 */
export function make(header: string, min_length: number, max_length: number): TextInput;
