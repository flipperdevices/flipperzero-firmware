import "source-map-support/register";

import { spawn, ChildProcess } from "child_process";
import net from "net";
import path from "path";

import pEvent from "p-event";
import temp from "temp";
import WebSocket from "ws";

const wss = new WebSocket.Server({
    port: 3001,
});

function generateSocketPath() {
    return temp.path({
        prefix: "flipper_emulator.",
        suffix: ".sock",
    });
}

class Context {
    fwProcess: ChildProcess | null = null;
    fwSocket: net.Socket | null = null;

    constructor(protected webSocket: WebSocket) {
        this.webSocket.on("message", message =>  {
            console.log("received", message);
            if (! Buffer.isBuffer(message)) {
                console.error("received non-binary message: ", message);
                return;
            }

            if (this.fwSocket) {
                this.fwSocket.write(message);
            }
        });

        this.webSocket.on("close", () => this.killFirmware());
        // TODO cleanup listeners, listening sockets

        this.startFirmware();
    }

    killFirmware() {
        if (this.fwProcess) {
            // TODO is it needed?
            // this.fwProcess.stdin.pause();
            this.fwProcess.kill();
            this.fwProcess = null;
        }
    }

    async startFirmware() {
        // There's a race between incoming socket close and awaiting in startFirmware
        // Sock can be close before this.fwProcess is filled, leading to process leaking
        // TODO fix it

        const socketPath = generateSocketPath();
        const server = net.createServer();
        server.listen(socketPath);

        await pEvent(server, "listening" as const);
        console.log(`Listening on ${socketPath}`);

        // TODO pass socket path via env var
        console.log("Spawning new firmware process");
        let fwProcess;
        try {
            // TODO pass path to executable via env var, or somehow
            fwProcess = this.fwProcess = spawn(
                path.join(
                    __dirname,
                    "..",
                    "..",
                    "..",
                    "target_lo",
                    "build",
                    "target_lo",
                ), [], {
                env: {
                    ...process.env,
                    "FLIPPER_EMULATOR_SERVER_PATH": socketPath,
                }
            });
        } catch (e) {
            console.error("failed to spawn new firmware process");
            console.error(e);
            // TODO communicate this to UI
            return;
        }
        fwProcess.stdout.on("data", (data) => {
            console.log(data.toString());
        });
        fwProcess.stderr.on("data", (data) => {
            console.log(data.toString());
        });

        // Beware, p-event has a typing issue, next line is not actually properly typed, as pEvent returns any
        this.fwSocket = await pEvent(server, "connection");
        this.setupFirmwareSocket();
    }

    setupFirmwareSocket() {
        this.fwSocket!.on("data", data => {
            this.webSocket.send(data);
        });
    }
}

wss.on("connection", webSocket => {
    console.log("new connection");
    new Context(webSocket);
});

console.log("Server started");
