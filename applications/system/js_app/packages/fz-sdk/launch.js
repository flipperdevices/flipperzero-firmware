#!/usr/bin/env node
import fs from "node:fs";
import { SerialPort } from "serialport";
import prompts from "prompts";

(async () => {
    const appFile = fs.readFileSync(process.argv[2]);
    const destPath = process.argv[3];

    const flippers = (await SerialPort.list()).filter(x => x.serialNumber?.startsWith("flip_"));

    if (!flippers) {
        console.error("No Flippers found");
        process.exit(1);
    }

    let portPath = flippers[0].path;
    if (flippers.length > 1) {
        port = (await prompts([{
            type: "select",
            name: "port",
            message: "Select Flipper to run the app on",
            choices: flippers.map(x => ({ title: x.serialNumber.replace("flip_", ""), value: x.path })),
        }])).port;
    }

    console.log(`Connecting to Flipper at ${portPath}`);
    let port = new SerialPort({ path: portPath, baudRate: 230400 });
    let received = "";
    let lastMatch = 0;
    async function waitFor(string, timeoutMs) {
        return new Promise((resolve, _reject) => {
            let timeout = undefined;
            if (timeoutMs) {
                timeout = setTimeout(() => {
                    console.error("Error: timeout");
                    process.exit(1);
                }, timeoutMs);
            }
            setInterval(() => {
                let idx = received.indexOf(string, lastMatch);
                if (idx !== -1) {
                    lastMatch = idx;
                    if (timeoutMs)
                        clearTimeout(timeout);
                    resolve();
                }
            }, 50);
        });
    }
    port.on("data", (data) => {
        received += data.toString();
    });

    await waitFor(">: ", 1000);
    console.log("Uploading application file");
    port.write(`storage remove ${destPath}\x0d`);
    port.drain();
    await waitFor(">: ", 1000);
    port.write(`storage write_chunk ${destPath} ${appFile.length}\x0d`);
    await waitFor("Ready", 1000);
    port.write(appFile);
    port.drain();
    await waitFor(">: ", 1000);

    console.log("Launching application");
    port.write(`js ${destPath}\x0d`);
    port.drain();

    await waitFor("Running", 1000);
    process.stdout.write(received.slice(lastMatch));
    port.on("data", (data) => {
        process.stdout.write(data.toString());
    });
    process.on("exit", () => {
        port.write("\x03");
    });

    await waitFor("Script done!", 0);
    process.exit(0);
})();
