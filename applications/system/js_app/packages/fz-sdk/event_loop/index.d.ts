/**
 * Module for dealing with events
 * 
 * ```js
 * let eventLoop = require("event_loop");
 * ```
 * 
 * The event loop is central to event-based programming in many frameworks, and
 * our JS subsystem is no exception. It is a good idea to familiarize yourself
 * with the event loop first before using any of the advanced modules (e.g. GPIO
 * and GUI).
 * 
 * # Conceptualizing the event loop
 * If you ever wrote JavaScript before, you have definitely seen callbacks. It's
 * when a function accepts another function (usually an anonymous one) as one of
 * the arguments, which it will call later on, e.g. when an event happens or
 * when data becomes ready:
 * ```js
 * setTimeout(function() { console.log("Hello, World!") }, 1000);
 * ```
 * 
 * Many JavaScript engines employ a queue that the runtime fetches events from
 * as they occur, subsequently calling the corresponding callbacks. This is done
 * in a long-running loop, hence the name "event loop". Here's the pseudocode
 * for a typical event loop:
 * ```js
 * while(loop_is_running()) {
 *     if(event_available_in_queue()) {
 *         let event = fetch_event_from_queue();
 *         let callback = get_callback_associated_with(event);
 *         if(callback)
 *             callback(get_extra_data_for(event));
 *     } else {
 *         // avoid wasting CPU time
 *         sleep_until_any_event_becomes_available();
 *     }
 * }
 * ```
 * 
 * Most JS runtimes enclose the event loop within themselves, so that most JS
 * programmers does not even need to be aware of its existence. This is not the
 * case with our JS subsystem.
 * 
 * # Example
 * This is how one would write something similar to the `setTimeout` example
 * above:
 * ```js
 * // import module
 * let eventLoop = require("event_loop");
 * 
 * // create an event source that will fire once 1 second after it has been created
 * let timer = eventLoop.timer("oneshot", 1000);
 * 
 * // subscribe a callback to the event source
 * eventLoop.subscribe(timer, function(_subscription, _item, eventLoop) {
 *     print("Hello, World!");
 *     eventLoop.stop();
 * }, eventLoop); // notice this extra argument. we'll come back to this later
 * 
 * // run the loop until it is stopped
 * eventLoop.run();
 * 
 * // the previous line will only finish executing once `.stop()` is called, hence
 * // the following line will execute only after "Hello, World!" is printed
 * print("Stopped");
 * ```
 * 
 * I promised you that we'll come back to the extra argument after the callback
 * function. Our JavaScript engine does not support closures (anonymous
 * functions that access values outside of their arguments), so we ask
 * `subscribe` to pass an outside value (namely, `eventLoop`) as an argument to
 * the callback so that we can access it. We can modify this extra state:
 * ```js
 * // this timer will fire every second
 * let timer = eventLoop.timer("periodic", 1000);
 * eventLoop.subscribe(timer, function(_subscription, _item, counter, eventLoop) {
 *     print("Counter is at:", counter);
 *     if(counter === 10)
 *         eventLoop.stop();
 *     // modify the extra arguments that will be passed to us the next time
 *     return [counter + 1, eventLoop];
 * }, 0, eventLoop);
 * ```
 * 
 * Because we have two extra arguments, if we return anything other than an
 * array of length 2, the arguments will be kept as-is for the next call.
 * 
 * The first two arguments that get passed to our callback are:
 *   - The subscription manager that lets us `.cancel()` our subscription
 *   - The event item, used for events that have extra data. Timer events do
 *     not, they just produce `undefined`.
 *
 * @version Added in JS SDK 0.1
 * @module
 */

/**
 * @ignore
 */
type Lit = undefined | null | {};

/**
 * Subscription control interface
 * @version Added in JS SDK 0.1
 */
export interface Subscription {
    /**
     * Cancels the subscription, preventing any future events managed by the
     * subscription from firing
     * @version Added in JS SDK 0.1
     */
    cancel(): void;
}

/**
 * Opaque event source identifier
 * @version Added in JS SDK 0.1
 */
export type Contract<Item = undefined> = symbol & { "__tag__": "contract" };
// introducing a nominal type in a hacky way; the `__tag__` property doesn't really exist.

/**
 * A callback can be assigned to an event loop to listen to an event. It may
 * return an array with values that will be passed to it as arguments the next
 * time that it is called. The first argument is always the subscription
 * manager, and the second argument is always the item that trigged the event.
 * The type of the item is defined by the event source.
 * @version Added in JS SDK 0.1
 */
export type Callback<Item, Args extends Lit[]> = (subscription: Subscription, item: Item, ...args: Args) => Args | undefined | void;

/**
 * Subscribes a callback to an event
 * @param contract Event identifier
 * @param callback Function to call when the event is triggered
 * @param args Initial arguments passed to the callback
 * @version Added in JS SDK 0.1
 */
export function subscribe<Item, Args extends Lit[]>(contract: Contract<Item>, callback: Callback<Item, Args>, ...args: Args): Subscription;
/**
 * Runs the event loop until it is stopped (potentially never)
 * @version Added in JS SDK 0.1
 */
export function run(): void | never;
/**
 * Stops the event loop
 * @version Added in JS SDK 0.1
 */
export function stop(): void;

/**
 * Creates a timer event that can be subscribed to just like any other event
 * @param mode Either `"oneshot"` or `"periodic"`
 * @param interval Timer interval in milliseconds
 * @version Added in JS SDK 0.1
 */
export function timer(mode: "oneshot" | "periodic", interval: number): Contract;

/**
 * Message queue
 * @version Added in JS SDK 0.1
 */
export declare class Queue<T> {
    /**
     * Message event
     * @version Added in JS SDK 0.1
     */
    input: Contract<T>;
    /**
     * Sends a message to the queue
     * @param message message to send
     * @version Added in JS SDK 0.1
     */
    send(message: T): void;
}

/**
 * Creates a message queue
 * @param length maximum queue capacity
 * @version Added in JS SDK 0.1
 */
export function queue<T>(length: number): Queue<T>;
