export interface Mode {
    direction: "in" | "out";
    outMode?: "push_pull" | "open_drain";
    inMode?: "analog" | "plain_digital" | "interrupt" | "event";
    edge?: "rising" | "falling" | "both";
    pull?: "up" | "down";
}

export interface Pin {
    /**
     * Configures a pin. This may be done several times.
     * @param mode Pin configuration object
     */
    init(mode: Mode): undefined;
    /**
     * Sets the output value of a pin if it's been configured with
     * `direction: "out"`.
     * @param value Logic value to output
     */
    write(value: boolean): undefined;
    /**
     * Gets the input value of a pin if it's been configured with
     * `direction: "in"`, but not `inMode: "analog"`.
     */
    read(): boolean;
}

/**
 * Returns an object that can be used to manage a GPIO pin. For the list of
 * available pins, see https://docs.flipper.net/gpio-and-modules#miFsS
 * @param pin Pin name (e.g. `"PC3"`) or number (e.g. `7`)
 */
export function get(pin: string | number): Pin;
