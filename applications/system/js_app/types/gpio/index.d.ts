export interface Mode {
    direction: "in" | "out";
    outMode?: "push_pull" | "open_drain";
    inMode?: "analog" | "plain_digital" | "interrupt" | "event";
    edge?: "rising" | "falling" | "both";
    pull?: "up" | "down";
}

type Lit = string | number | boolean | undefined | null | void | {};

export interface Pin {
    /**
     * Configures a pin. This may be done several times.
     * @param mode Pin configuration object
     */
    init(mode: Mode): void;
    /**
     * Sets the output value of a pin if it's been configured with
     * `direction: "out"`.
     * @param value Logic value to output
     */
    write(value: boolean): void;
    /**
     * Gets the input value of a pin if it's been configured with
     * `direction: "in"`, but not `inMode: "analog"`.
     */
    read(): boolean;
    /**
     * Gets the input voltage of a pin in millivolts if it's been configured
     * with `direction: "in"` and `inMode: "analog"`
     */
    read_analog(): number;
    /**
     * Attaches a callback to a pin if it's been configured with
     * `inMode: "interrupt"` or `"event"`. The triggering edge is specified with
     * the `edge` mode parameter. Any previously attached handlers are detached.
     * @param handler Function to call back when the interrupt is triggered
     * @param args Additional arguments to supply to the handlers
     */
    attach_handler<T extends Lit[]>(handler: (pin: Pin, ...args: T) => void, ...args: T): void;
    /**
     * Detaches a previously attached callback from a pin.
     */
    detach_handler(): void;
}

/**
 * Returns an object that can be used to manage a GPIO pin. For the list of
 * available pins, see https://docs.flipper.net/gpio-and-modules#miFsS
 * @param pin Pin name (e.g. `"PC3"`) or number (e.g. `7`)
 */
export function get(pin: string | number): Pin;
/**
 * Calls the handlers of the interrupts that have happened since the last call
 * to this function.
 * @param block If set to true, this function will not return unless at least
 * one interrupt is processed.
 */
export function process_interrupts(block: boolean): void;
