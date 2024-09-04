import type { View } from ".";

/**
 * Simple loading screen
 */
export class Loading implements View {
    _view: symbol;
}

export function make(): Loading;
