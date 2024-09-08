import type { View } from ".";

/**
 * Simple loading screen with a hourglass
 */
export class Loading implements View {
    _view: symbol;
}

export function make(): Loading;
