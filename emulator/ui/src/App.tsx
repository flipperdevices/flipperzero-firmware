import React from 'react';
import './App.css';
import {FrontPanel, Props as FrontPanelProps} from './FrontPanel';

export type PwmStates = {
    red: number,
    green: number,
    blue: number,
};

type Props = {
    onReset?: () => void,
} & FrontPanelProps;

const App: React.FC<Props> = (props) => {
    const {
        onReset,
    } = props;

    return (
        <div className="App">
            <p className="App-intro">
                Flipper Zero emulator
            </p>
            <div
                style={{
                    "display": "flex",
                    "flexDirection": "column",
                    "alignItems": "center"
                }}
            >
                <FrontPanel/>
                <div
                    style={{
                        "display": "flex",
                        "flexDirection": "row",
                    }}
                >
                    <div
                        style={{
                            "display": "flex",
                            "flexDirection": "column",
                        }}
                    >
                        <span>TTY</span>
                        <textarea readOnly={true}></textarea>
                        <div>
                            Send:<input/>
                        </div>
                    </div>
                    <div
                        style={{
                            "display": "flex",
                            "flexDirection": "column",
                        }}
                    >
                        <span>FW working</span>
                        Last update: 2 min ago
                        <button onClick={onReset}>Reset</button>
                    </div>
                </div>
            </div>
        </div>
    );
};

export default App;
