type Lit = undefined | null | {};

export interface Subscription {
    cancel(): void;
}

export type Contract = symbol;

export type Callback<Args extends Lit[]> = (subscription: Subscription, ...args: Args) => Args | void;

export function subscribe<Args extends Lit[]>(contract: Contract, callback: Callback<Args>, ...args: Args): Subscription;
export function run(): void;
export function stop(): void;

export function timer(mode: "oneshot" | "periodic", interval: number): Contract;
