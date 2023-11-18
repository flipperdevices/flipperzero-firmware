export const NUM_COLUMNS = 16;

export const stringConverter = {
  convertByte: function (oneByte, bytesPerPixel, conversionType, endianness) {
    let stringByte = "???";

    if (conversionType === "HEX0" || conversionType === "HEX_SLASH") {
      stringByte = oneByte.toString(16).padStart(bytesPerPixel * 2, "0");
      if (endianness === "be") {
        stringByte = this.changeEndianness(stringByte);
      }
      stringByte = (conversionType === "HEX0" ? "0x" : "\\x") + stringByte;
    } else if (conversionType === "DEC") {
      stringByte = oneByte;
    } else if (conversionType === "BIN") {
      stringByte = "B" + oneByte.toString(2).padStart(bytesPerPixel * 8, "0");
    } else {
      console.error("Unknown conversion type");
    }

    return stringByte;
  },

  convert: function (
    dataLength,
    bytesPerPixel,
    conversionType,
    multiLine,
    endianness,
    colNumber,
    data
  ) {
    let resultString = "";
    for (let i = 0; i < dataLength; i++) {
      let stringByte = "";
      // Need to use BigInt so we can use 32-bit integers (4 bytes per pixel)
      let combinedByte = BigInt("0b00000000000000000000000000000000");
      for (let j = 0; j < bytesPerPixel; j++) {
        let pixelByte = BigInt(data[i * bytesPerPixel + j]);
        if (j !== 0) {
          combinedByte = combinedByte << BigInt(8);
        }
        combinedByte = combinedByte | pixelByte;
      }
      stringByte =
        this.convertByte(
          combinedByte,
          bytesPerPixel,
          conversionType,
          endianness
        ) + ", ";
      if (multiLine && (i + 1) % colNumber === 0) {
        stringByte += "\r\n  ";
      }

      resultString += stringByte;
    }
    resultString = resultString.substr(0, resultString.lastIndexOf(",")).trim();
    return resultString;
  },

  changeEndianness: function (val) {
    const result = [];
    let len = val.length - 2;
    while (len >= 0) {
      result.push(val.substr(len, 2));
      len -= 2;
    }
    return result.join("");
  },
};

export function prepareBinary(data) {
  return Buffer.from(data);
}

export function convertToString(data, colNum, filename) {
  let resultString = "";
  const conversionType = "HEX0";
  const endianness = "le";
  const dataLength = data.length;
  const bytesPerPixel = 1;

  const actualDataLength = dataLength / bytesPerPixel;
  const colNumber = colNum;
  const multiLine = true;

  resultString += `// array size is ${dataLength}\r\n`;
  resultString += `#define ${filename}_len ${dataLength}\n\n`;
  resultString += `static const uint8_t ${filename}[] PROGMEM = {\r\n  `;
  resultString += stringConverter.convert(
    actualDataLength,
    bytesPerPixel,
    conversionType,
    multiLine,
    endianness,
    colNumber,
    data
  );
  resultString += "\r\n};";

  return resultString;
}
