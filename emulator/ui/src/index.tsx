import {assertNever} from "assert-never";
import React from 'react';
import ReactDOM from 'react-dom';

import './index.css';
import App, {PwmStates} from './App';
import {
    CenterType,
    Channel1, Channel2, Channel4,
    Connection,
    DeviceOutput,
    DisplayData,
    DisplayDataType, HTIM3,
    HTIM4, LeftType,
    PwmDataType, RightType,
} from './connection';
import * as serviceWorker from './serviceWorker';

// TODO rewrite on redux
// TODO null for initial values?
let pwmStates : PwmStates = {
    red: 0,
    green: 0,
    blue: 0,
};

const connection = new Connection((output:DeviceOutput) => {
    switch (output[0]) {
        case PwmDataType:
            const [[timer], [channel], value] = output[1];
            if (timer === HTIM4 && channel === Channel1) {
                pwmStates.red = value;
            } else if (timer === HTIM4 && channel === Channel2) {
                pwmStates.green = value;
            } else if (timer === HTIM3 && channel === Channel4) {
                pwmStates.blue = value;
            }
            break;
        case DisplayDataType:
            handleDisplayData(output[1][0]);
            break;
        default:
            assertNever(output[0]);
    }

    render();
});

let ledStates : Array<Array<boolean>> = [];
for (let digit = 0; digit<8; digit+=1) {
    ledStates[digit] = [];
    for (let segment = 0; segment<8; segment+=1) {
        ledStates[digit][segment] = false;
    }
}
let ledIntensity : number = 0;

function handleDisplayData(data:DisplayData) {
    if (data.length < 2) {
        return;
    }

    const [reg, value] = data;

    switch (reg) {
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
            const bits = value
                .toString(2)
                .padStart(8, '0')
                .split('')
                .reverse()
                .map(_ => parseInt(_))
                .map(Boolean)
            ;
            ledStates[reg - 1] = bits;
            break;
        case 0x0A:
            ledIntensity = value;
            break;
        default:
            console.log("Unimplemented register", reg);
            break;
    }
}

const onReset = () => connection.reset();
const onCenter = () => connection.send([CenterType, []]);
const onLeft = () => connection.send([LeftType, []]);
const onRight = () => connection.send([RightType, []]);

const root = document.getElementById('root') as HTMLElement;
if (root == null) {
    throw new Error("App root not found");
}

function render() {
    ReactDOM.render(
        <React.StrictMode>
            <App
                onReset={onReset}
                onCenter={onCenter}
                onLeft={onLeft}
                onRight={onRight}
            />
        </React.StrictMode>,
        root
    );
}

render();

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
