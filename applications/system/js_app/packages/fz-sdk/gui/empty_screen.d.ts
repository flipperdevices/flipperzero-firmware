/**
 * @file Empty screen GUI view
 * @version Added in JS SDK 0.1
 */

import type { View, ViewFactory } from ".";

type Props = {};
declare class EmptyScreen extends View<Props> { }
declare class EmptyScreenFactory extends ViewFactory<Props, EmptyScreen> { }
declare const factory: EmptyScreenFactory;
export = factory;
