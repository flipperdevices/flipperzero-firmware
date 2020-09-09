import React from "react";

export type Props = {
    onUp?: () => void,
    onLeft?: () => void,
    onCenter?: () => void,
    onRight?: () => void,
    onDown?: () => void,
    onBack?: () => void,
}

export const FrontPanel: React.FC<Props> = (props) => {
    const zoom = (v:number):number => v*2;

    const {
        onUp,
        onLeft,
        onCenter,
        onRight,
        onDown,
        onBack,
    } = props;

    return (
        <div
            style={{
                "display": "flex",
                "flexDirection": "row",
                "alignItems": "end"
            }}
        >
            {/* TODO replace all this with svg panel drawing */}
            <div
                className="device-display"
                style={{
                    "background": "#ff8000",
                    "border": "1px solid black",
                    "width": zoom(128),
                    "height": zoom(64),
                }}
            />
            <div
                className="device-led"
                style={{
                    "background": "#ff0000",
                    "border": "1px solid black",
                    "borderRadius": zoom(8),
                    "width": zoom(16),
                    "height": zoom(16),
                }}
            />
            <div className="dpad">
                {/* TODO maybe grid? */}
                <div className="buttons">
                    <div className="button-col">
                        <button id="up_button" className="button" onClick={onUp}>△</button>
                    </div>
                </div>
                <div className="buttons">
                    <div className="button-col">
                        <button id="left_button" className="button" onClick={onLeft}>◁</button>
                        <button id="center_button" className="button" onClick={onCenter}>ok</button>
                        <button id="right_button" className="button" onClick={onRight}>▷</button>
                    </div>
                </div>
                <div className="buttons">
                    <div className="button-col">
                        <button id="down_button" className="button" onClick={onDown}>▽</button>
                    </div>
                </div>
            </div>
            <div className="back">
                <div className="buttons">
                    <div className="button-col">
                        <button id="back_button" className="button" onClick={onBack}>&lt;</button>
                    </div>
                </div>
            </div>
        </div>
    );
};
