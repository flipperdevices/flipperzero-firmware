<template>
  <div class="flex justify-center items-center w-full h-full bg-gray-200 overflow-hidden pattern">
  <div class="flex flex-col w-full h-full ">
    <div ref="contentDiv" class="h-1/3 flex items-center p-4" style="background-color: #929d97; ">
      <table ref="table" align="center" class="table w-full h-1/3">
      <tbody>
        <tr v-for="y in 8" :key="`row-${y}`">
          <td
            v-for="x in 48"
            :key="`col-${x}`"
            class="border border-gray-300"
            @mousedown="mouseDownHandler"
            @mouseover="mouseOverHandler"
          ></td>
        </tr>
      </tbody>
    </table>
  </div>
  <div class="flex-none mx-2 my-2 ">
  <div class="sm:w-1/2 w-full mx-auto bg-white p-4 rounded-lg border shadow-md">
    <div class="flex my-2">
      <Button @click="invertLogo" class="mr-2">Invert</Button>
      <Button @click="clearLogo" class="mr-2">Clear</Button>
      <Button @click="resetLogo" class="mr-2">Reset</Button>
      <Button @click="flipperLogo" class="mr-2">Flipper</Button>
      <Button @click="hackedLogo" >Hacked</Button>
    </div>
    <div align="center">
      <form>
        <div class="row">
          <div class="flex items-center mb-2">
            <label for="titleInput" class="w-44 mr-2">Title</label>
            <Input v-model="title" placeholder="11 ASCII characters" maxlength="11" />
          </div>
          <div class="flex items-center mb-2">
            <label for="manufacturerInput" class="w-44 mr-2">Manufacturer</label>
            <Input v-model="manufacturer" placeholder="4 character manufacturer code" maxlength="4" />
          </div>
          <div class="flex items-center mb-2">
            <label for="cgbSupportSelect" class="w-44 mr-2">CGB?</label>
            <Select v-model="cgbSupportSelect">
              <option value="" disabled>Select CGB Support</option>
              <option value="00">No CGB Support</option>
              <option value="80">CGB Support</option>
              <option value="C0">CGB Only</option>
            </Select>
          </div>
          <div class="flex items-center mb-2">
            <label for="newLicenseeInput" style="white-space: nowrap" class="w-44 mr-2"
              >New Licensee Code</label
            >
            <Input v-model="newLicensee" placeholder="2 character ASCII code, indicating company or publisher" maxlength="2" />
          </div>
          <div class="flex items-center mb-2">
            <label for="sgbSelect" class="w-44 mr-2">SGB?</label>
            <Select v-model="sgbSelect">
              <option value="00">No</option>
              <option value="03">Yes</option>
            </Select>
          </div>
          <div class="flex items-center mb-2">
            <label for="cartridgeTypeSelect" class="w-44 mr-2">Mapper</label>
            <Select v-model="cartridgeTypeSelect">
              <option value="" disabled>Select mapper</option>
              <option value="00">ROM ONLY</option>
              <option value="01">MBC1</option>
              <option value="02">MBC1+RAM</option>
              <option value="03">MBC1+RAM+BATTERY</option>
              <option value="05">MBC2</option>
              <option value="06">MBC2+BATTERY</option>
              <option value="08">ROM+RAM</option>
              <option value="09">ROM+RAM+BATTERY</option>
              <option value="0B">MMM01</option>
              <option value="0C">MMM01+RAM</option>
              <option value="0D">MMM01+RAM+BATTERY</option>
              <option value="0F">MBC3+TIMER+BATTERY</option>
              <option value="10">MBC3+TIMER+RAM+BATTERY</option>
              <option value="11">MBC3</option>
              <option value="12">MBC3+RAM</option>
              <option value="13">MBC3+RAM+BATTERY</option>
              <option value="15">MBC4</option>
              <option value="16">MBC4+RAM</option>
              <option value="17">MBC4+RAM+BATTERY</option>
              <option value="19">MBC5</option>
              <option value="1A">MBC5+RAM</option>
              <option value="1B">MBC5+RAM+BATTERY</option>
              <option value="1C">MBC5+RUMBLE</option>
              <option value="1D">MBC5+RUMBLE+RAM</option>
              <option value="1E">MBC5+RUMBLE+RAM+BATTERY</option>
              <option value="20">MBC6</option>
              <option value="22">MBC7+SENSOR+RUMBLE+RAM+BATTERY</option>
              <option value="FC">POCKET CAMERA</option>
              <option value="FD">BANDAI TAMA5</option>
              <option value="FE">HuC3</option>
              <option value="FF">HuC1+RAM+BATTERY</option>
            </Select>
          </div>
        </div>
        <div class="row">
          <div class="flex items-center mb-2">
            <label for="romSizeSelect" class="w-44 mr-2">ROM Size</label>
            <Select v-model="romSizeSelect">
              <option value="" disabled>Select ROM size</option>
              <option value="00">32KByte (no ROM banking)</option>
              <option value="01">64KByte (4 banks)</option>
              <option value="02">128KByte (8 banks)</option>
              <option value="03">256KByte (16 banks)</option>
              <option value="04">512KByte (32 banks)</option>
              <option value="05">1MByte (64 banks)</option>
              <option value="06">2MByte (128 banks)</option>
              <option value="07">4MByte (256 banks)</option>
              <option value="52">1.1MByte (72 banks)</option>
              <option value="53">1.2MByte (80 banks)</option>
              <option value="54">1.5MByte (96 banks)</option>
            </Select>
          </div>
          <div class="flex items-center mb-2">
            <label for="ramSizeSelect" class="w-44 mr-2">RAM Size</label>
            <Select v-model="ramSizeSelect">
              <option value="" disabled>Select RAM size</option>
              <option value="00">No RAM</option>
              <option value="01">2KBytes</option>
              <option value="02">8KBytes</option>
              <option value="03">32KBytes</option>
              <option value="04">128KBytes</option>
              <option value="05">64KBytes</option>
            </Select>
          </div>
          <div class="flex items-center mb-2">
            <label for="destinationSelect" class="w-44 mr-2">Japan?</label>
            <Select v-model="destinationSelect">
              <option value="01">No</option>
              <option value="00">Yes</option>
            </Select>
          </div>
          <div class="flex items-center mb-2">
            <label for="oldLicenseeInput" class="w-44 mr-2">Old Licensee Code</label>
            <Input
              v-model="oldLicenseeInput"
              type="text"
              placeholder="2 hex symbols"
              maxlength="2"
            />
          </div>
          <div class="flex items-center mb-2">
            <label for="versionNumberInput" class="w-44 mr-2">Mask ROM Version</label>
            <Input
              v-model="versionNumberInput"
              type="text"
              placeholder="2 hex symbols"
            />
          </div>
        </div>
      </form>
      <div align="center" class="flex">
        <Button class="mr-2">
          Upload ROM
          <input
            type="file"
            accept=".gb, .gbc"
            style="display: none"
            class="inputFile"
            @change="onUploadROM"
          />
          <div id="result"></div>
        </Button>
        <Button
          type="button"
          class="btn btn-primary btn-lg"
          @click="downloadFile"
        >
          Download ROM
        </Button>
      </div>
      <a download="logo.gb" id="downloadlink" style="display: none">Download</a>
    </div>
  </div>
  </div> 
</div>
</div>
</template>
<script>
import Button from "./components/Button.vue";
import Input from "./components/Input.vue";
import Select from './components/Select.vue';
/*
//
// MODIFICATIONS TO STRING CLASS
//
*/

// Gets the ASCII equivalent of two hex characters
String.prototype.getASCIIFromHex = function () {
  let returnString = "";
  // read the hex string two characters at a time
  for (i = 0; i < this.length; i += 2) {
    // ignore 00 ASCII characters
    if (this.substr(i, 2) !== "00") {
      returnString += String.fromCharCode(parseInt(this.substr(i, 2), 16));
    }
  }
  return returnString;
};

// Encodes a (UNICODE?) string to hex values
String.prototype.hexEncode = function () {
  var hex, i;

  var result = "";
  for (i = 0; i < this.length; i++) {
    // hex will be a hex character, but could possibly be only one digit when we need it to be two
    hex = this.charCodeAt(i).toString(16).toUpperCase();
    if (hex.length == 1) {
      result += "0";
    }
    result += hex;
  }
  return result;
};

String.prototype.isValidASCII = function () {
  return /^[\x00-\x7F]*$/.test(this);
};

String.prototype.toHexString = function () {
  let returnString = "";
  for (i = 0; i < this.length; i++) {
    returnString += this.charCodeAt(i).toString(16).toUpperCase();
  }
  return returnString;
};

// Checks if a decimal integer can be converted to a hex value
String.prototype.isValidHexString = function () {
  let i, k;
  var validValues = [
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
  ];
  // go through the entire string
  for (i = 0; i < this.length; i++) {
    // check each character in string against array
    for (k = 0; k < validValues.length; k++) {
      // if a match is found get out of loop
      if (validValues[k] === this.charAt(i)) {
        break;
      }
    }
    // if the characters match then restart loop
    if (validValues[k] === this.charAt(i)) {
      break;
    }
    return false;
  }
  return true;
};

const LOGO_FLIPPER_HEX =
  "1367EE0CCCCC1100EECCFFCFDDD9FF9F89B3FF0E7766EE667666C000CCF700DDCCEEFCCC1111E8883333E0FF7666C666";
const LOGO_HACKED_HEX = "0000667733ff0003000e0007111989be0003000c0007cccc0000666633336663666eccc7d1d9ceb967636e0eccc7cccc";
const LOGO_HEX =
  "CEED6666CC0D000B03730083000C000D0008111F8889000EDCCC6EE6DDDDD999BBBB67636E0EECCCDDDC999FBBB9333E";
let uploadedHexData =
  "C38B020000000000C38B02FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF87E15F1600195E2356D5E1E9FFFFFFFFFFFFFFFFFFFFFFFFC3FD01FFFFFFFFFFC31227FFFFFFFFFFC31227FFFFFFFFFFC37E01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00C35001CEED6666CC0D000B03730083000C000D0008111F8889000EDCCC6EE6DDDDD999BBBB67636E0EECCCDDDC999FBBB9333E";
for (var i = 0; i < 32460; i++) {
  uploadedHexData += "FF";
}

export default {
  components: {
    Button,
    Input,
    Select
  },
  data() {
    return {
      title: "",
      manufacturer: "",
      cgbSupportSelect: "00",
      newLicensee: "",
      sgbSelect: "00",
      cartridgeTypeSelect: "00",
      romSizeSelect: "00",
      ramSizeSelect: "00",
      destinationSelect: "01",
      oldLicenseeInput: "",
      versionNumberInput: "",
      mouseDown: false,
      blackFlag: false,
      counter: 0,
    };
  },
  mounted() {
    window.addEventListener("resize", this.setTableDimensions);
    document.addEventListener("mousedown", this.onMouseDown);
    document.addEventListener("mouseup", this.onMouseUp);
    // document.addEventListener("touchstart", this.onMouseDown);
    // document.addEventListener("touchend", this.onMouseUp);
    this.$nextTick(() => {
      this.setTableDimensions();
      this.loadLogo(LOGO_HEX);
    });
  },
  beforeDestroy() {
    window.removeEventListener("resize", this.setTableDimensions);
    document.removeEventListener("mousedown", this.onMouseDown);
    document.removeEventListener("mouseup", this.onMouseUp);

    // document.removeEventListener("touchstart", this.onMouseDown);
    // document.removeEventListener("touchend", this.onMouseUp);
  },
  methods: {
    // Gets the title hex based on title input
    getTitle() {
      let text = this.title;
      // Do checks
      if (text.length > 0 && text.isValidASCII()) {
        // First convert to hex string
        let returnString = text.toHexString();
        // And then if length is less than 22, fill in remaining hex characters with 0's
        for (i = returnString.length; i < 22; i++) {
          returnString += "0";
        }
        return returnString;
      } else if (text.length == 0) {
        return "0000000000000000000000";
      } else {
        return null;
      }
    },
    // Gets the manufacturer ID hex based on the manufacturer input
    getManufacturerCode() {
      let text = this.manufacturer;
      // Do checks
      if (text.length == 4 && text.isValidASCII()) {
        return text.toHexString();
      } else if (text.length == 0) {
        return "00000000";
      } else {
        return null;
      }
    },
    // Gets the cgb select box hex data based on its value
    getCGBFlag() {
      return this.cgbSupportSelect;
    },
    // Gets the new licensee hex data based on new licnesee input
    getNewLicenseeCode() {
      let text = this.newLicensee;
      // Do checks
      if (text.length == 2 && text.isValidASCII()) {
        return text.toHexString();
      } else if (text.length == 0) {
        return "0000";
      } else {
        return null;
      }
    },
    // Gets the sgb hex based on checkbox
    getSGBFlag() {
      return this.sgbSelect;
    },
    // Gets hex data based on cartridge type selected
    getCartridgeType() {
      return this.cartridgeTypeSelect;
    },
    // Gets hex data based on ROM selection
    getRomSize() {
      return this.romSizeSelect;
    },
    // Gets hex data based on the RAM size select box
    getRamSize() {
      return this.ramSizeSelect;
    },
    // Gets hex data based on the destination checkbox
    getDestinationCode() {
      return this.destinationSelect;
    },
    // Gets the old licnesee code hex based on the old licnesee input
    getOldLicenseeCode() {
      let text = this.oldLicenseeInput;
      // Do checks
      if (text.length == 2 && text.isValidHexString()) {
        return text;
      } else if (text.length == 0) {
        return "00";
      } else {
        return null;
      }
    },
    // Gets the rom verion number hex based on the version number input
    getRomVersionNumber() {
      let text = this.versionNumberInput;
      // Do checks
      if (text.length == 2 && text.isValidHexString()) {
        return text;
      } else if (text.length == 0) {
        return "00";
      } else {
        return null;
      }
    },
    // Retrieves the values of all fields and concatenates them into a single hex string
    // If there are any errors, simply displays a popupbox instead and returns "NULL"
    getFieldValues() {
      // First get data
      var hexData = "";
      var title = this.getTitle();
      var manufacturerCode = this.getManufacturerCode();
      var cgbFlag = this.getCGBFlag();
      var newLicenseeCode = this.getNewLicenseeCode();
      var sgbFlag = this.getSGBFlag();
      var cartridgeType = this.getCartridgeType();
      var romSize = this.getRomSize();
      var ramSize = this.getRamSize();
      var destinationCode = this.getDestinationCode();
      var oldLicenseeCode = this.getOldLicenseeCode();
      var romVersionNumber = this.getRomVersionNumber();
      var errorString = "";
      // Check for invalid inputs
      if (title === null) {
        errorString = "Input for title was invalid\n";
      }
      if (manufacturerCode === null) {
        errorString += "Input for manufacturer code was invalid\n";
      }
      if (cgbFlag === null) {
        errorString += "Input for cgb flag was invalid\n";
      }
      if (newLicenseeCode === null) {
        errorString += "Input for new licensee code was invalid\n";
      }
      if (sgbFlag === null) {
        errorString += "Input for sgb flag was invalid\n";
      }
      if (cartridgeType === null) {
        errorString += "Input for cartridge type was invalid\n";
      }
      if (romSize === null) {
        errorString += "Input for rom size was invalid\n";
      }
      if (ramSize === null) {
        errorString += "Input for ram size was invalid\n";
      }
      if (destinationCode === null) {
        errorString += "Input for destination code was invalid\n";
      }
      if (oldLicenseeCode === null) {
        errorString += "Input for old licensee code was invalid\n";
      }
      if (romVersionNumber === null) {
        errorString += "Input for rom version number was invalid\n";
      }
      if (errorString !== "") {
        document.getElementById("alertModalBody").innerText = errorString;
        $("#alertModal").modal();
        // alert(errorString);
        return null;
      } else {
        hexData += title;
        hexData += manufacturerCode;
        hexData += cgbFlag;
        hexData += newLicenseeCode;
        hexData += sgbFlag;
        hexData += cartridgeType;
        hexData += romSize;
        hexData += ramSize;
        hexData += destinationCode;
        hexData += oldLicenseeCode;
        hexData += romVersionNumber;
        return hexData;
      }
    },
    // Performs a bitwise not operation on a decimal integer
    invert(x) {
      return 15 - x;
    },
    // precondition: string is the hex representation of bytes of data
    // with NO spaces
    // Calculates the checksum for the header data
    calculateHeaderChecksum(hexString) {
      var totalChecksum = "";
      var firstChecksum = 0;
      var secondChecksum = 0;
      var first = 0;
      var second = 0;
      var carry = 0;
      for (let x = 0; x < hexString.length; x += 2) {
        // reset carry bit
        carry = 0;
        // get the hex values for a byte
        var first = parseInt("0x" + hexString[x]);
        var second = parseInt("0x" + hexString[x + 1]);
        // invert them
        first = this.invert(first);
        second = this.invert(second);
        // sum the least significant 4 bits
        secondChecksum += second;
        // if this result is greater than 15, then we have a carry bit
        if (secondChecksum > 15) {
          secondChecksum -= 16;
          carry = 1;
        }
        firstChecksum += first + carry;
        // if this result is greater than 15, then we have a carry bit
        // but we don't care about it
        if (firstChecksum > 15) {
          firstChecksum -= 16;
        }
      }
      // at the very end, convert the checksum ints to chars and put them
      // together. NOTE: checksum should always be a single byte
      totalChecksum += firstChecksum.toString(16).toUpperCase();
      totalChecksum += secondChecksum.toString(16).toUpperCase();
      return totalChecksum;
    },
    downloadROM(fieldData) {
      var hexData = ""; // this is binary representation of file
      var fieldData = this.getFieldValues(); // if this is null then there was an error
      if (fieldData === null) {
        return;
      }
      // if there was hex data uploaded, inject the modifications into that
      if (uploadedHexData.length > 0) {
        // pre-header stuff
        hexData = uploadedHexData.substr(0, 520);
        hexData += this.convertLogoToHex();
        hexData += fieldData;
        // calculate header checksums
        hexData += this.calculateHeaderChecksum(fieldData);
        // calculate global checksums
        hexData += this.calculateGlobalChecksum(
          hexData + uploadedHexData.substr(668, uploadedHexData.length)
        );
        // post-header stuff
        hexData += uploadedHexData.substr(672, uploadedHexData.length);
      } else {
        // otherwise, just create some garbage data
        hexData =
          "C38B020000000000C38B02FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF87E15F1600195E2356D5E1E9FFFFFFFFFFFFFFFFFFFFFFFFC3FD01FFFFFFFFFFC31227FFFFFFFFFFC31227FFFFFFFFFFC37E01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00C35001";
        hexData += this.convertLogoToHex();
        hexData += fieldData;
        hexData += this.calculateHeaderChecksum(fieldData);
        // fill up the rest of the 32kb rom with 0xFF
        for (var i = 0; i < 32432; i++) {
          hexData += "FF";
        }
      }

      let byteArray = new Uint8Array(hexData.length / 2);
      for (let x = 0; x < byteArray.length; x++) {
        byteArray[x] = parseInt(hexData.substr(x * 2, 2), 16);
      }

      let data = new Blob([byteArray], { type: "application/octet-stream" });

      let textFile = window.URL.createObjectURL(data);

      // create the download link, then download it, then destroy it
      let a = document.createElement("a");
      let clickEvent = new MouseEvent("click", {
        view: window,
        bubbles: true,
        cancelable: false,
      });
      a.style = "display: none";
      a.href = textFile;
      // check the cgb box to see if the rom should have .gb or .gbc extension
      let name = this.title;
      // make sure there is a name
      if (name === "") {
        name = "logo";
      }
      if (this.cgbSupportSelect == "00") {
        a.download = name + ".gb";
      } else {
        a.download = name + ".gbc";
      }
      a.dispatchEvent(clickEvent);
      setTimeout(function () {
        // document.body.removeChild(a);
        window.URL.revokeObjectURL(textFile);
      }, 100);
    },

    // Calculates the global checksum based on a hex string
    // The header checksum is included in this
    calculateGlobalChecksum(string) {
      var totalChecksum = "";
      var firstChecksum = 0;
      var secondChecksum = 0;
      var thirdChecksum = 0;
      var fourthChecksum = 0;
      var first = 0;
      var second = 0;
      var carry = 0;

      for (let x = 0; x < string.length; x += 2) {
        // if (x != 1002 && x != 1005){
        if (x != 668 && x != 670) {
          first = parseInt("0x" + string[x + 1]);
          second = parseInt("0x" + string[x]);
          firstChecksum += first;
          if (firstChecksum > 15) {
            firstChecksum -= 16;
            carry = 1;
          } else {
            carry = 0;
          }
          secondChecksum += second + carry;
          if (secondChecksum > 15) {
            secondChecksum -= 16;
            carry = 1;
          } else {
            carry = 0;
          }
          thirdChecksum += carry;
          if (thirdChecksum > 15) {
            thirdChecksum -= 16;
            carry = 1;
          } else {
            carry = 0;
          }
          fourthChecksum += carry;
          if (fourthChecksum > 15) {
            fourthChecksum -= 16;
          }
        }
      }
      totalChecksum += fourthChecksum.toString(16).toUpperCase();
      totalChecksum += thirdChecksum.toString(16).toUpperCase();
      totalChecksum += secondChecksum.toString(16).toUpperCase();
      totalChecksum += firstChecksum.toString(16).toUpperCase();

      return totalChecksum;
    },

    // Changes the color of all cells
    invertLogo() {
      let list = this.$refs.table.getElementsByTagName("TD");
      for (let x = 0; x < list.length; x++) {
        this.fill(list[x]);
      }
    },
    // Blanks the logo
    clearLogo() {
      let list = this.$refs.table.getElementsByTagName("TD");
      for (let x = 0; x < list.length; x++) {
        list[x].style.backgroundColor = "transparent";
      }
    },
    // Resets the logo to the default "Nintendo"
    resetLogo() {
      this.loadLogo(LOGO_HEX);
    },
    // Loads a hex representation of a logo into the editor
    loadLogo(hexData) {
      var row = new Array(4);
      let list = this.$refs.table.getElementsByTagName("TD");

      if (!hexData) {
        hexData = prompt(
          "Please enter the hex data. Whitespace is ignored.",
          LOGO_HEX
        );
      }
      // first make sure hexData is the properlength and eliminate whitespace in the string
      if (!hexData.isValidHexString()) {
        alert("The hex data contained invalid characters!");
        return;
      }
      if (hexData.length == 96) {
        this.clearLogo();
        // first do top half of logo
        for (let x = 0; x < 48; x += 4) {
          // convert 2 bytes of data
          row[0] = parseInt("0x" + hexData[x]);
          row[1] = parseInt("0x" + hexData[x + 1]);
          row[2] = parseInt("0x" + hexData[x + 2]);
          row[3] = parseInt("0x" + hexData[x + 3]);
          for (let y = 0; y < 4; y++) {
            // set first bit
            if (Math.floor(row[y] / 8) == 1) {
              list[x + y * 48].style.backgroundColor = "black";
              row[y] -= 8;
            }
            // then second bit
            if (Math.floor(row[y] / 4) == 1) {
              list[x + 1 + y * 48].style.backgroundColor = "black";
              row[y] -= 4;
            }
            // then third bit
            if (Math.floor(row[y] / 2) == 1) {
              list[x + 2 + y * 48].style.backgroundColor = "black";
              row[y] -= 2;
            }
            // then fourth bit
            if (Math.floor(row[y] / 1) == 1) {
              list[x + 3 + y * 48].style.backgroundColor = "black";
            }
          }
        }
        // then do bottom half
        for (let x = 48; x < 96; x += 4) {
          // convert 2 bytes of data
          row[0] = parseInt("0x" + hexData[x]);
          row[1] = parseInt("0x" + hexData[x + 1]);
          row[2] = parseInt("0x" + hexData[x + 2]);
          row[3] = parseInt("0x" + hexData[x + 3]);
          for (let y = 0; y < 4; y++) {
            // set first bit
            if (Math.floor(row[y] / 8) == 1) {
              list[144 + x + y * 48].style.backgroundColor = "black";
              row[y] -= 8;
            }
            // then second bit
            if (Math.floor(row[y] / 4) == 1) {
              list[145 + x + y * 48].style.backgroundColor = "black";
              row[y] -= 4;
            }
            // then third bit
            if (Math.floor(row[y] / 2) == 1) {
              list[146 + x + y * 48].style.backgroundColor = "black";
              row[y] -= 2;
            }
            // then fourth bit
            if (Math.floor(row[y] / 1) == 1) {
              list[147 + x + y * 48].style.backgroundColor = "black";
            }
          }
        }
      } else {
        // CONVERT TO MODAL
        alert("The hex data received was not the correct length!");
      }
    },
    onMouseDown() {
      this.mouseDown = true;
    },
    onMouseUp() {
      this.mouseDown = false;
    },
    mouseDownHandler(event) {
      this.fill(event.target);
      this.setBlackFlag(event.target);
    },
    mouseOverHandler(event) {
      if (this.mouseDown) {
        if (this.blackFlag) {
          event.target.style.backgroundColor = "black";
        } else {
          event.target.style.backgroundColor = "transparent";
        }
      }
    },
    // When the black flag is set over an element...
    setBlackFlag(element) {
      if (element.style.backgroundColor == "black") {
        this.blackFlag = true;
      } else {
        this.blackFlag = false;
      }
    },
    // Fill the element
    fill(element) {
      if (element.style.backgroundColor == "black") {
        element.style.backgroundColor = "transparent";
      } else {
        element.style.backgroundColor = "black";
      }
    },

    setTableDimensions() {
      let width = this.$refs.table.offsetWidth * 0.1651376146789;
      this.$refs.table.style.height = `${width}px`;
      // this.$refs.contentDiv.style.height = `${this.$refs.table.style.height}px`;
    },

    // Gets the state of the logo, and coverts it to a hex string
    convertLogoToHex() {
      let list = this.$refs.table.getElementsByTagName("TD");
      let hexString = "";
      let toptop = 0;
      let topbottom = 0;
      let bottomtop = 0;
      let bottombottom = 0;
      // there are 48 blocks total
      for (let block = 0; block < 24; block++) {
        toptop = 0;
        topbottom = 0;
        bottomtop = 0;
        bottombottom = 0;
        // each block has 4 columns
        for (let column = 0; column < 4; column++) {
          // block % 12 gives us which column of blocks we are in
          // Math.floor(block / 12) gives us which row of blocks we are in
          // if we are in the column n + 1 then we want to start 4 cells passed column n
          // if we are in row n + 1 then we want to start 96 cells passed row n
          // first add top number of top block
          let bgColor =
            list[(block % 12) * 4 + Math.floor(block / 12) * 192 + column].style
              .backgroundColor;
          if (bgColor == "black") {
            // Math.pow(2, 3 - column) will give us 2^3 for the first column, and so on
            toptop += Math.pow(2, 3 - column);
          }
          // then bottom number of top block
          bgColor =
            list[(block % 12) * 4 + Math.floor(block / 12) * 192 + column + 48]
              .style.backgroundColor;
          if (bgColor == "black") {
            topbottom += Math.pow(2, 3 - column);
          }
          // then top number of bottom block
          bgColor =
            list[(block % 12) * 4 + Math.floor(block / 12) * 192 + column + 96]
              .style.backgroundColor;
          if (bgColor == "black") {
            bottomtop += Math.pow(2, 3 - column);
          }
          // then bottom number of bottom block
          bgColor =
            list[(block % 12) * 4 + Math.floor(block / 12) * 192 + column + 144]
              .style.backgroundColor;
          if (bgColor == "black") {
            bottombottom += Math.pow(2, 3 - column);
          }
        }
        // the bottom is added to the string first, then the top
        hexString += toptop.toString(16).toUpperCase();
        hexString += topbottom.toString(16).toUpperCase();
        hexString += bottomtop.toString(16).toUpperCase();
        hexString += bottombottom.toString(16).toUpperCase();
      }
      return hexString;
    },
    readFile(file, callback) {
      let reader = new FileReader();
      reader.onload = callback;
      // make sure user didn't hit cancel
      if (file != null) {
        reader.readAsBinaryString(file);
      }
    },
    // Clears the logo and everything else
    clearEverything() {
      this.clearLogo();
      this.title = "";
      this.manufacturer = "";
      this.cgbSupportSelect = "00";
      this.newLicensee = "";
      this.sgbSelect = "00";
      this.cartridgeTypeSelect = "00";
      this.romSizeSelect = "00";
      this.ramSizeSelect = "00";
      this.destinationSelect = "01";
      this.oldLicenseeInput = "";
      this.versionNumberInput = "";
    },
    // Sets the cgb select box based on hex data
    setCGBFlag(cgbFlag) {
      this.cgbSupportSelect = cgbFlag;
    },
    // Gets the new lisencee code based on hex data
    setNewLicenseeCode(code) {
      if (code === "0000") {
        this.newLicensee = "NA";
      } else {
        this.newLicensee = code.getASCIIFromHex();
      }
    },
    // Sets the sgb checkbox based on hex data
    setSGBFlag(sgbFlag) {
      this.sgbSelect = sgbFlag;
    },
    // Sets the cartridge type based on hex data
    setCartridgeType(cartridgeType) {
      this.cartridgeTypeSelect = cartridgeType;
    },
    // Sets the ROM size based on hex data
    setRomSize(romSize) {
      this.romSizeSelect = romSize;
    },
    // Sets the RAM size select box based on hex data
    setRamSize(ramSize) {
      this.ramSizeSelect = ramSize;
    },
    // Sets the destination checkbox based on hex data
    setDestinationCode(destinationCode) {
      this.destinationSelect = destinationCode;
    },
    // From the uploaded rom file, update the UI
    parseUploadedHexString(hexString) {
      // first clear everything
      this.clearEverything();
      // then set variables
      let nintendoLogo = hexString.substr(520, 96);
      let title = hexString.substr(616, 22);
      let manufacturerCode = hexString.substr(638, 8);
      let cgbFlag = hexString.substr(646, 2);
      let newLicenseeCode = hexString.substr(648, 4);
      let sgbFlag = hexString.substr(652, 2);
      let cartridgeType = hexString.substr(654, 2);
      let romSize = hexString.substr(656, 2);
      let ramSize = hexString.substr(658, 2);
      let destinationCode = hexString.substr(660, 2);
      let oldLicenseeCode = hexString.substr(662, 2);
      let romVersionNumber = hexString.substr(664, 2);

      // then update the UI
      this.loadLogo(nintendoLogo);
      this.title = title.getASCIIFromHex();
      this.manufacturer = manufacturerCode.getASCIIFromHex();
      //setManufacturerCode(manufacturerCode);
      //document.getElementById('manufacturerInput').value = manufacturerCode.getASCIIFromHex();
      this.setCGBFlag(cgbFlag);
      this.setNewLicenseeCode(newLicenseeCode);
      //document.getElementById('newLicenseeInput').value = newLicenseeCode.getASCIIFromHex();
      this.setSGBFlag(sgbFlag);
      this.setCartridgeType(cartridgeType);
      this.setRomSize(romSize);
      this.setRamSize(ramSize);
      this.setDestinationCode(destinationCode);
      this.oldLicenseeInput = oldLicenseeCode;
      this.versionNumberInput = romVersionNumber;
    },
    onUploadROM(event) {
      this.readFile(event.target.files[0], (e) => {
        //manipulate with result...
        uploadedHexData = e.target.result.hexEncode();
        this.parseUploadedHexString(uploadedHexData);
      });
    },
    downloadFile() {
      // First check field values to make sure they are okay
      let fieldData = this.getFieldValues();
      if (fieldData === null) {
        return;
      }
      let downloadOverride = false; // if the logo data isn't okay, stops the download
      // Check the logo
      if (this.convertLogoToHex() !== LOGO_HEX) {
        downloadOverride = true;
        // $('#confirmationModal').modal();
        this.downloadROM();
      }
      if (!downloadOverride) {
        downloadROM(fieldData);
      }
    },
    flipperLogo() {
      this.loadLogo(LOGO_FLIPPER_HEX);
    },
    hackedLogo() {
      this.loadLogo(LOGO_HACKED_HEX)
    }
  },
};
</script>

<style scoped>
table {
  display: table;
  /* border-collapse: separate; */
  box-sizing: border-box;
  text-indent: initial;
  /* border-color: #ddd; */

  border-collapse: separate;
  text-indent: initial;
  border-spacing: 0.05rem;
}
td,
th {
  border: 1px solid rgba(0,0,0, 0.3);
}
td {
  display: table-cell;
  vertical-align: inherit;
}
</style>
