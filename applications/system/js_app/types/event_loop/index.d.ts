type Lit = undefined | null | {};

/**
 * Subscription control interface
 */
export interface Subscription {
    /**
     * Cancels the subscription, preventing any future events managed by the
     * subscription from firing
     */
    cancel(): void;
}

/**
 * Opaque event source identifier
 */
export type Contract = symbol;

/**
 * A callback can be assigned to an event loop to listen to an event. It may
 * return an array with values that will be passed to it as arguments the next
 * time that it is called.
 */
export type Callback<Args extends Lit[]> = (subscription: Subscription, ...args: Args) => Args | undefined | void;

/**
 * Subscribes a callback to an event
 * @param contract Event identifier
 * @param callback Function to call when the event is triggered
 * @param args Initial arguments passed to the callback
 */
export function subscribe<Args extends Lit[]>(contract: Contract, callback: Callback<Args>, ...args: Args): Subscription;
/**
 * Runs the event loop until it is stopped (potentially never)
 */
export function run(): void | never;
/**
 * Stops the event loop
 */
export function stop(): void;

/**
 * Creates a timer event that can be subscribed to just like any other event
 * @param mode Either `"oneshot"` or `"periodic"`
 * @param interval Timer interval in milliseconds
 */
export function timer(mode: "oneshot" | "periodic", interval: number): Contract;
