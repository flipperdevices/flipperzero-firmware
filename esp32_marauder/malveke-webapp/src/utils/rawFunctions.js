// Functions in this file are taken from https://www.npmjs.com/package/gbp-decode
// For simplicity they are mirrored in this file

const COMMAND_INIT = 0x1;
const COMMAND_PRINT = 0x2;
const COMMAND_DATA = 0x4;
const COMMAND_STATUS = 0xf;

const STATE_AWAIT_MAGIC_BYTES = 0;
const STATE_AWAIT_COMMAND = 1;
const STATE_AWAIT_COMPRESSION_INFO = 2;
const STATE_AWAIT_PACKET_DATA_LENGTH = 3;
const STATE_AWAIT_DATA = 4;
const STATE_AWAIT_CHECKSUM = 5;
const STATE_AWAIT_KEEPALIVE = 6;
const STATE_AWAIT_STATUS_QUERY = 7;

const MODE_DETECT_LENGTH = 0;
const MODE_COMPRESSED = 1;
const MODE_UNCOMPRESSED = 2;

const EMPTY_PACKET = {
  command: null,
  buffer: [],
  data: [],
  hasCompression: null,
  dataLength: 0,
  checksum: 0,
};

const toByteArray = (fileContents) => {
  return fileContents
    .split("\n")
    .filter((line) => !line.startsWith("//"))
    .map((line) => line.trim())
    .map((line) =>
      line
        .split(" ")
        .filter(Boolean)
        .map((cc) => parseInt(cc, 16))
    )
    .flat();
};

const parsePackets = (bytes) => {
  let state = STATE_AWAIT_MAGIC_BYTES;
  let packet = { ...EMPTY_PACKET };
  const packets = [];

  bytes.forEach((byte) => {
    switch (state) {
      case STATE_AWAIT_MAGIC_BYTES:
        if (packet.buffer.length === 0 && byte === 0x88) {
          packet.buffer.push(byte);
          return;
        } else if (packet.buffer.length === 1 && byte === 0x33) {
          packet.buffer = [];
          state = STATE_AWAIT_COMMAND;
          return;
        } else {
          packet = { ...EMPTY_PACKET };
          return;
        }

      case STATE_AWAIT_COMMAND:
        packet.command = byte;
        state = STATE_AWAIT_COMPRESSION_INFO;
        return;

      case STATE_AWAIT_COMPRESSION_INFO:
        packet.hasCompression = byte;
        state = STATE_AWAIT_PACKET_DATA_LENGTH;
        return;

      case STATE_AWAIT_PACKET_DATA_LENGTH:
        if (packet.buffer.length === 0) {
          packet.buffer.push(byte);
          return;
        }

        packet.dataLength = packet.buffer[0] + (byte << 8);
        packet.buffer = [];

        if (packet.dataLength === 0) {
          state = STATE_AWAIT_CHECKSUM;
        } else {
          state = STATE_AWAIT_DATA;
        }
        return;

      case STATE_AWAIT_DATA:
        if (packet.buffer.length < packet.dataLength) {
          packet.buffer.push(byte);
          return;
        }
        packet.data = packet.buffer;
        packet.buffer = [];
        state = STATE_AWAIT_CHECKSUM;
        return;

      case STATE_AWAIT_CHECKSUM:
        if (packet.buffer.length === 0) {
          packet.buffer.push(byte);
          return;
        }
        packet.checksum = packet.buffer[0] + (byte << 8);
        packet.buffer = [];
        state = STATE_AWAIT_KEEPALIVE;
        return;

      case STATE_AWAIT_KEEPALIVE:
        state = STATE_AWAIT_STATUS_QUERY;
        return;

      case STATE_AWAIT_STATUS_QUERY:
        state = STATE_AWAIT_MAGIC_BYTES;
        delete packet.buffer;
        packets.push(packet);
        packet = { ...EMPTY_PACKET };
        return;
    }
  });

  return packets;
};

const getImageDataStream = (packets) => {
  return packets.filter(
    ({ command }) => command === COMMAND_DATA || command === COMMAND_PRINT
  );
};

const unpack = (data) => {
  const dataOut = [];

  let mode = MODE_DETECT_LENGTH;
  let length = 0;

  data.forEach((byte) => {
    switch (mode) {
      case MODE_DETECT_LENGTH:
        // noinspection JSBitwiseOperatorUsage
        if (byte & 0x80) {
          mode = MODE_COMPRESSED;
          length = (byte & 0x7f) + 2;
        } else {
          mode = MODE_UNCOMPRESSED;
          length = byte + 1;
        }
        return;

      case MODE_UNCOMPRESSED:
        length -= 1;
        if (length === 0) {
          mode = MODE_DETECT_LENGTH;
        }
        dataOut.push(byte);
        return;

      case MODE_COMPRESSED:
        dataOut.push(...[...Array(length)].map(() => byte));
        mode = MODE_DETECT_LENGTH;
        length = 0;
        return;
    }
  });

  return dataOut;
};

const decompressDataStream = (packets) => {
  return packets.map((packet) => {
    if (packet.command === COMMAND_DATA) {
      return {
        ...packet,
        hasCompression: 0,
        data: packet.hasCompression ? unpack(packet.data) : packet.data,
      };
    }

    return packet;
  });
};

const parsePaletteByte = (paletteRaw) => {
  return [
    (paletteRaw >> 6) & 0x3,
    (paletteRaw >> 4) & 0x3,
    (paletteRaw >> 2) & 0x3,
    (paletteRaw >> 0) & 0x3,
  ];
};

const decodePrintCommands = (packets) => {
  return packets.map((packet) => {
    if (packet.command === COMMAND_PRINT) {
      return {
        ...packet,
        data: {
          margins: packet.data[1],
          marginUpper: packet.data[1] >> 4,
          marginLower: packet.data[1] & 0xf,
          palette: packet.data[2],
          paletteData: parsePaletteByte(packet.data[2]),
        },
      };
    }

    return packet;
  });
};

const harmonizePalette = (charA, charB, paletteDefinition = [3, 2, 1, 0]) => {
  const bits = [...Array(8)].map((_, index) => ({
    a: (charB >> (7 - index)) % 2,
    b: (charA >> (7 - index)) % 2,
  }));

  const res = bits
    .map(({ a, b }) => (a << 1) + b)
    // .map(plg)
    .map((val) => paletteDefinition[3 - val])
    // .map(plg)
    .map((mapped) => ({
      a: (mapped >> 1) % 2,
      b: mapped % 2,
    }))
    .reduce(
      (acc, current, index) => ({
        a: acc.a + (current.a << (7 - index)),
        b: acc.b + (current.b << (7 - index)),
      }),
      {
        a: 0,
        b: 0,
      }
    );

  return [res.b & 0xff, res.a & 0xff];
};

const harmonizePalettes = (packets) => {
  const unharmonizedPackets = [];

  return packets.map((packet) => {
    switch (packet.command) {
      case COMMAND_DATA:
        unharmonizedPackets.push(packet);
        break;

      case COMMAND_PRINT:
        while (unharmonizedPackets.length) {
          let unharmonizedPacket = unharmonizedPackets.shift();
          const data = [];

          for (let i = 0; i < unharmonizedPacket.data.length; i += 2) {
            data.push(
              ...harmonizePalette(
                unharmonizedPacket.data[i],
                unharmonizedPacket.data[i + 1],
                packet.data.paletteData
              )
            );
          }

          Object.assign(unharmonizedPacket, { data });
        }
        break;
    }

    return packet;
  });
};

const transformToClassic = (packets) => {
  let image = {
    transformed: [],
    palette: null,
  };

  let currentLine = [];
  const images = [];

  packets.forEach((packet) => {
    switch (packet.command) {
      case COMMAND_DATA:
        for (let i = 0; i < packet.data.length; i += 1) {
          currentLine.push(packet.data[i].toString(16).padStart(2, "0"));
          if (i % 16 === 15) {
            image.transformed.push(currentLine.join(" "));
            currentLine = [];
          }
        }
        break;

      case COMMAND_PRINT:
        image.palette = packet.data.paletteData || image.palette;

        if (packet.data.marginLower !== 0) {
          console.log(packet.data.marginLower);

          images.push(image.transformed);

          image = {
            transformed: [],
          };
          currentLine = [];
        }

        break;

      default:
        break;
    }
  });

  if (image.transformed.length) {
    images.push(image.transformed);
  }

  return images;
};
