import type { View } from ".";

/**
 * Empty screen
 */
export class EmptyScreen implements View {
    _view: symbol;
}

export function make(): EmptyScreen;
