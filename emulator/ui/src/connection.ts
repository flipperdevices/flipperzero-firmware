import BufferList from 'bl';
import msgpack5 from 'msgpack5';

// TODO setup variable name in tslint for snake_case, to ease rust transition

// Foo(u16, u16) -> [1/*type*/, [7/*first u16*/, 12/*second*/]]
// Bar -> [2/*type*/, []]

type HTIM3 = 0;
export const HTIM3 : HTIM3 = 0;

type HTIM4 = 1;
export const HTIM4 : HTIM4 = 1;

type PwmTimer = HTIM3 | HTIM4;
type PwmTimerEnum = [PwmTimer, []];

type Channel1 = 0;
export const Channel1 : Channel1 = 0;

type Channel2 = 1;
export const Channel2 : Channel2 = 1;

type Channel3 = 2;
export const Channel3 : Channel3 = 2;

type Channel4 = 3;
export const Channel4 : Channel4 = 3;

type PwmChannel = Channel1 | Channel2 | Channel3 | Channel4;
type PwmChannelEnum = [PwmChannel, []];

type PwmDataType = 0;
export const PwmDataType : PwmDataType = 0;
type PwmDataPacket = [PwmDataType, [PwmTimerEnum, PwmChannelEnum, number]];

export type DisplayData = Array<number>;

type DisplayDataType = 1;
export const DisplayDataType : DisplayDataType = 1;
type DisplayDataPacket = [DisplayDataType, [DisplayData]];

export type DeviceOutput = PwmDataPacket | DisplayDataPacket;

type CenterType = 0;
export const CenterType : CenterType = 0;
type CenterInput = [CenterType, []];

type LeftType = 1;
export const LeftType : LeftType = 1;
type LeftInput = [LeftType, []];

type RightType = 2;
export const RightType : RightType = 2;
type RightInput = [RightType, []];

type DeviceInput = CenterInput | LeftInput | RightInput;

// TODO separate framing from device-specific messages
export class Connection {
    mp = msgpack5();
    bl = new BufferList();
    socket: WebSocket | null = null;

    constructor(protected onOutput: (output:DeviceOutput) => void) {}

    processBuffer(packetBL:BufferList) {
        // Next line is unsafe - this.mp.decode returns any
        // TODO validate known packets via io-ts
        const packet: DeviceOutput = this.mp.decode(packetBL);
        this.onOutput(packet);
    }

    process() {
        while(true) {
            if (this.bl.length < 4) {
                return;
            }

            const length = this.bl.readUInt32LE(0);
            if (this.bl.length < 4 + length) {
                return;
            }

            this.bl.consume(4);
            // msgpack5 can decode from bl, using shallowSlice
            const packetBL = this.bl.shallowSlice(0, length);
            this.bl.consume(length);

            this.processBuffer(packetBL);
        }
    }

    send(input:DeviceInput) {
        if (! this.socket) {
            throw new Error("Socket is not connected");
        }

        const encoded = this.mp.encode(input);
        const lenBuf = new Buffer(4);
        lenBuf.writeUInt32LE(encoded.length, 0);
        const fbl = new BufferList();
        fbl.append(lenBuf);
        fbl.append(encoded);
        this.socket.send(fbl.slice());
    }

    reset() {
        if (this.socket) {
            this.socket.close();
        }

        this.socket = new WebSocket('http://127.0.0.1:3001/');
        this.socket.binaryType = "arraybuffer";

        this.socket.addEventListener('connect',() => {
            console.log("connected");
        });

        this.socket.addEventListener("message", ({data}) => {
            if (!(data instanceof ArrayBuffer)) {
                console.error('received non-binary message: ', data);
                return;
            }
            const buffer = new Buffer(data);
            this.bl.append(buffer);
            this.process();
        });
    }
}

