/**
 * @file Text input GUI view
 * @version Added in JS SDK 0.1
 */

import type { View, ViewFactory } from ".";
import type { Contract } from "../event_loop";

type Props = {
    header: string,
    minLength: number,
    maxLength: number,
}
declare class TextInput extends View<Props> {
    input: Contract<string>;
}
declare class TextInputFactory extends ViewFactory<Props, TextInput> { }
declare const factory: TextInputFactory;
export = factory;
