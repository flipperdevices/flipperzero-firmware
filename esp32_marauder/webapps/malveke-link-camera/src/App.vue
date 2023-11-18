<script>
import { nextTick, onBeforeUnmount, onMounted, ref } from "vue";
import GameBoyColor from "./components/GameBoyColor.vue";
import HexInput from "./components/HexInput.vue";
import Button from './components/Button.vue';
import Select from './components/Select.vue';

let interval = true;

// Tile Constants
const TILE_PIXEL_WIDTH = 8;
const TILE_PIXEL_HEIGHT = 8;
const TILES_PER_LINE = 20; // Gameboy Printer Tile Constant

const initLine = '{"command":"INIT"}';
const moreLine = '{"command":"DATA", "compressed":0, "more":1}';
const finalLine = '{"command":"DATA","compressed":0,"more":0}';
const terminatorLine =
  '{"command":"PRNT","sheets":1,"margin_upper":1,"margin_lower":3,"pallet":228,"density":64 }';

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

export default {
  // data() {
  //   return {
  //     dumpText: ''
  //   }
  // },
  components: { HexInput, GameBoyColor, Button, Select },
  setup() {
    const container = ref(null);
    const dumpText = ref("");
    const canvasRef = ref(null);
    const isTransfered = ref(false);
    const loading = ref(false);
    const palette = ref("grafixkidgreen");
    const tiles = ref([]);
    const from = ref(0);
    const bufferData = ref([]);

    // This paints the tile with a specified offset and pixel width
    function paint(
      canvas,
      pixels,
      pixel_width,
      pixel_height,
      tile_x_offset,
      tile_y_offset
    ) {
      let colors = [];
      switch (palette.value) {
        case "grayscale":
          colors = new Array(
            "#ffffff",
            "#aaaaaa",
            "#555555",
            "#000000",
            "#FFFFFF00"
          );
          break;
        case "dmg":
          colors = new Array(
            "#9BBC0F",
            "#77A112",
            "#306230",
            "#0F380F",
            "#FFFFFF00"
          );
          break;
        case "gameboypocket":
          colors = new Array(
            "#c4cfa1",
            "#8b956d",
            "#4d533c",
            "#1f1f1f",
            "#FFFFFF00"
          );
          break;
        case "gameboycoloreuus":
          colors = new Array(
            "#ffffff",
            "#7bff30",
            "#0163c6",
            "#000000",
            "#FFFFFF00"
          );
          break;
        case "gameboycolorjp":
          colors = new Array(
            "#ffffff",
            "#ffad63",
            "#833100",
            "#000000",
            "#FFFFFF00"
          );
          break;
        case "bgb":
          colors = new Array(
            "#e0f8d0",
            "#88c070",
            "#346856",
            "#081820",
            "#FFFFFF00"
          );
          break;
        case "grafixkidgray":
          colors = new Array(
            "#e0dbcd",
            "#a89f94",
            "#706b66",
            "#2b2b26",
            "#FFFFFF00"
          );
          break;
        case "grafixkidgreen":
          colors = new Array(
            "#dbf4b4",
            "#abc396",
            "#7b9278",
            "#4c625a",
            "#FFFFFF00"
          );
          break;
        case "blackzero":
          colors = new Array(
            "#7e8416",
            "#577b46",
            "#385d49",
            "#2e463d",
            "#FFFFFF00"
          );
          break;
        default:
          colors = new Array(
            "#ffffff",
            "#aaaaaa",
            "#555555",
            "#000000",
            "#FFFFFF00"
          );
      }
      const tile_offset = tile_x_offset * tile_y_offset;
      const pixel_x_offset = TILE_PIXEL_WIDTH * tile_x_offset * pixel_width;
      const pixel_y_offset = TILE_PIXEL_HEIGHT * tile_y_offset * pixel_height;

      var ctx = canvas.getContext("2d");

      // pixels along the tile's x axis
      for (var i = 0; i < TILE_PIXEL_WIDTH; i++) {
        for (var j = 0; j < TILE_PIXEL_HEIGHT; j++) {
          // pixels along the tile's y axis

          // Pixel Color
          ctx.fillStyle = colors[pixels[j * TILE_PIXEL_WIDTH + i]];

          // Pixel Position (Needed to add +1 to pixel width and height to fill in a gap)
          ctx.fillRect(
            pixel_x_offset + i * pixel_width,
            pixel_y_offset + j * pixel_height,
            pixel_width + 1,
            pixel_height + 1
          );
        }
      }
    }
    function renderImage(img) {
      tiles.value = img;
      var tile_height_count = Math.floor(tiles.value.length / TILES_PER_LINE);
      const canvas = canvasRef.value;
      // /* Determine size of each pixel in canvas */
      var square_width = canvas.width / (TILE_PIXEL_WIDTH * TILES_PER_LINE);
      var square_height = square_width;

      // Resize height (Setting new canvas size will reset canvas)
      canvas.width = square_width * TILE_PIXEL_WIDTH * TILES_PER_LINE;
      let h = square_height * TILE_PIXEL_HEIGHT * tile_height_count;
      if (h > 0) {
        canvas.height = h;
      }

      tiles.value.forEach(function (pixels, index) {
        // Gameboy Tile Offset
        var tile_x_offset = index % TILES_PER_LINE;
        var tile_y_offset = Math.floor(index / TILES_PER_LINE);
        paint(
          canvas,
          pixels,
          square_width,
          square_height,
          tile_x_offset,
          tile_y_offset
        );
      });
    }

    // Gameboy tile decoder function from http://www.huderlem.com/demos/gameboy2bpp.html
    function decode(rawBytes) {
      var bytes = rawBytes.replace(/[^0-9A-F]/gi, "");
      if (bytes.length != 32) return false;

      var byteArray = new Array(16);
      for (var i = 0; i < byteArray.length; i++) {
        byteArray[i] = parseInt(bytes.substr(i * 2, 2), 16);
      }

      var pixels = new Array(TILE_PIXEL_WIDTH * TILE_PIXEL_HEIGHT);
      for (var j = 0; j < TILE_PIXEL_HEIGHT; j++) {
        for (var i = 0; i < TILE_PIXEL_WIDTH; i++) {
          var hiBit = (byteArray[j * 2 + 1] >> (7 - i)) & 1;
          var loBit = (byteArray[j * 2] >> (7 - i)) & 1;
          pixels[j * TILE_PIXEL_WIDTH + i] = (hiBit << 1) | loBit;
        }
      }
      return pixels;
    }
    function onImport() {
      try {
        var rawBytes = dumpText.value;

        // rawBytes is a string of hex where each line represents a gameboy tile
        var tiles_rawBytes_array = [
          initLine,
          ...rawBytes.split(/\n/),
          finalLine,
          terminatorLine,
        ];

        var images = [];
        var currentImage = null;

        tiles_rawBytes_array
          .map(function (raw_line, line_number) {
            if (raw_line.charAt(0) === "#") return null;

            if (raw_line.charAt(0) === "/") return null;

            if (raw_line.charAt(0) === "{") {
              try {
                var command = JSON.parse(raw_line.slice(0).trim());
                if (command.command === "INIT") {
                  return "INIT";
                } else if (command.command === "PRNT") {
                  return command.margin_lower;
                }
              } catch (error) {
                throw new Error(
                  "Error while trying to parse JSON data block in line " +
                    (1 + line_number)
                );
              }
            }
            return decode(raw_line);
          })
          .filter(Boolean)
          .forEach(function (tile_element) {
            if (tile_element === "INIT" && currentImage) {
              // ignore init if image has not finished 'printing'
            } else if (tile_element === "INIT" && !currentImage) {
              currentImage = [];
            } else if (typeof tile_element === "number") {
              // handle margin value
              var margin_lower = tile_element;

              // if margin is 3 split into new image 'finish printing'
              if (margin_lower === 3) {
                images.push(currentImage);
                currentImage = [];
              }
              // otherwise feed blank lines using lower margin value
              else {
                var blank_tile = new Array(64).fill(4);
                // send 40 blank tiles per margin feed
                for (i = 0; i < margin_lower * 40; i++) {
                  currentImage.push(blank_tile);
                }
              }
            } else {
              try {
                currentImage.push(tile_element);
              } catch (error) {
                throw new Error(
                  "No image start found. Maybe this line is missing"
                );
              }
            }
          });
        images.forEach((img) => renderImage(img));
      } catch (error) {
        console.error(error.message);
      }
    }
    function onChangePalette() {
      if (isTransfered.value || dumpText.value) {
        onImport();
      }
    }
    async function onRestart() {
      try {
        interval = false;
        setTimeout(async () => {
          const response = await fetch("/reset");
          if (response.ok) {
            const data = await response.json();
            setTimeout(() => {
              window.location.reload(1);
            }, 1000);
          } else {
            console.error("Request failed with status:", response.status);
          }
        }, 1000);
      } catch (error) {
        console.error(error);
      }
    }
    function parseBuffer() {
      const groupedData = [];
      const groupSize = 16;
      let buff = bufferData.value.map(num => num.toString(16).toUpperCase().padStart(2,'0'));
      for (let i = 0; i < buff.length; i += groupSize) {
        groupedData.push(buff.slice(i, i + groupSize).join(" "));
      }

      return groupedData.join("\n");
    }
    async function requestData() {
      try {
        const response = await fetch(`/image?from=${from.value}`);
        if (response.ok) {
          const data = await response.json();
          loading.value = true;
          from.value = data.from + data.dump.length;
          bufferData.value = bufferData.value.concat(data.dump);
          if (data.printed == "1") {
            loading.value = false;
            console.log("Listo!");
            interval = false;
            dumpText.value = parseBuffer();
            isTransfered.value = true;
            onImport();
          } else {
            dumpText.value = parseBuffer();
            onImport();
          }
        } else {
          console.error("Request failed with status:", response.status);
        }
      } catch (error) {
        console.error("An error occurred:", error);
        // interval = false;
      }
    }

    async function initPool() {
      while (interval) {
        await requestData();
        await sleep(1000);
      }
    }

    function getRGBValue(pixels, index) {
      const palette = [0xffffff, 0xaaaaaa, 0x555555, 0x000000];
      const value = palette[pixels[index]];

      return {
        // eslint-disable-next-line no-bitwise
        r: (value & 0xff0000) >> 16,
        // eslint-disable-next-line no-bitwise
        g: (value & 0x00ff00) >> 8,
        // eslint-disable-next-line no-bitwise
        b: value & 0x0000ff,
      };
    }
    function paintTileScaled(
      pixels,
      index,
      canvasContext,
      pixelSize,
      tilesPerLine
    ) {
      const tileXOffset = index % tilesPerLine;
      const tileYOffset = Math.floor(index / tilesPerLine);

      const pixelXOffset = TILE_PIXEL_WIDTH * tileXOffset * pixelSize;
      const pixelYOffset = TILE_PIXEL_HEIGHT * tileYOffset * pixelSize;

      // pixels along the tile's x axis
      for (let x = 0; x < TILE_PIXEL_WIDTH; x += 1) {
        for (let y = 0; y < TILE_PIXEL_HEIGHT; y += 1) {
          // pixels along the tile's y axis

          const color = getRGBValue(pixels, y * TILE_PIXEL_WIDTH + x);
          // eslint-disable-next-line no-param-reassign
          canvasContext.fillStyle = `rgb(${color.r}, ${color.g}, ${color.b})`;

          // Pixel Position (Needed to add +1 to pixel width and height to fill in a gap)
          canvasContext.fillRect(
            pixelXOffset + x * pixelSize,
            pixelYOffset + y * pixelSize,
            pixelSize + 1,
            pixelSize + 1
          );
        }
      }
    }

    function onPressButton(btn) {
      switch (btn) {
        case "A":
          const scaleFactor = 2;
          const canvas = canvasRef.value;
          // const context = canvas.getContext('2d');

          // const initialHeight = TILE_PIXEL_HEIGHT * Math.ceil(tiles.value.length / TILES_PER_LINE);
          // const   initialWidth = TILES_PER_LINE * TILE_PIXEL_WIDTH;
          // const   tilesPerLine = TILES_PER_LINE;

          // canvas.width = initialWidth * scaleFactor;
          // canvas.height = initialHeight * scaleFactor;

          // paintTileScaled(tiles.value, 0, context, scaleFactor, tilesPerLine);
          let link = document.createElement("a");
          link.download = `gb-camera-${new Date().getTime()}.png`;
          link.href = canvas.toDataURL();
          link.click();

          break;
      }
    }
    function setDimensions() {
      // let scale = this.$refs.gameboy.offsetWidth * 0.1651376146789;
      // console.log('scale',scale)

      const windowWidth = window.innerWidth;
      const windowHeight = window.innerHeight;
      
      // Calcula la escala para que el elemento se ajuste al tamaño de la ventana
      // const scaleX = windowWidth / 350; // 350px es el ancho original
      const scale = windowHeight / 720; // 570px es el alto original
      // const scale = Math.min(scaleX, scaleY);
      
      // Aplica la escala al elemento
      if(windowHeight < 800)  {
        container.value.style.transform = `scale(${scale})`;
      }
      
    };

    onMounted(() => {

      initPool();
      window.addEventListener("resize", setDimensions);
      nextTick(() => {
        setDimensions();
      });
    });
    onBeforeUnmount(() => {
      window.removeEventListener("resize", setDimensions);
    });
    return {
      loading,
      palette,
      dumpText,
      canvasRef,
      isTransfered,
      onImport,
      onRestart,
      onChangePalette,
      onPressButton,
      container
    };
  },
};
</script>

<template>
  <div class="flex justify-center items-center w-full h-full bg-gray-200 overflow-hidden pattern">
    <div class="flex flex-row h-auto justify-between w-full md:w-1/2">
      <div ref="container" class="flex flex-col w-full mr-1 flex-1 m-auto pb-8">
        <div
          class="flex flex-col items-start justify-between  xl:hidden bg-white rounded-md p-2 mb-4 mx-12 border border-gray-300 shadow-md"
        >
          <div class="text-xs font-bold mr-4 mb-2">Color Palette</div>
          <Select
            v-model="palette"
            @change="onChangePalette">
            <option value="grayscale">Grayscale</option>
            <option value="dmg">Original Game Boy</option>
            <option value="gameboypocket">Game Boy Pocket</option>
            <option value="gameboycoloreuus">
              Game Boy Color (Game Boy Camera, EU/US)
            </option>
            <option value="gameboycolorjp">
              Game Boy Color (PocketCamera, JP)
            </option>
            <option value="bgb">bgb emulator</option>
            <option value="grafixkidgray">Grafixkid Gray</option>
            <option value="grafixkidgreen">Grafixkid Green</option>
            <option value="blackzero">Game Boy (Black Zero) palette</option>
          </Select>
        </div>
        <GameBoyColor @press-button="onPressButton">
          <canvas ref="canvasRef" style="width: 210px; height: 190px"></canvas>
        </GameBoyColor>
      </div>

      <div class="flex-col ml-1 w-96 hidden xl:flex" style="height:580px">
        <div
          class="flex flex-col items-start justify-between bg-white rounded-md p-2 mb-2 border border-gray-300 shadow-md"
        >
        <div class="text-xs font-bold mr-4 mb-2">Color Palette</div>
          <Select v-model="palette" @change="onChangePalette">
            <option value="grayscale">Grayscale</option>
            <option value="dmg">Original Game Boy</option>
            <option value="gameboypocket">Game Boy Pocket</option>
            <option value="gameboycoloreuus">
              Game Boy Color (Game Boy Camera, EU/US)
            </option>
            <option value="gameboycolorjp">
              Game Boy Color (PocketCamera, JP)
            </option>
            <option value="bgb">bgb emulator</option>
            <option value="grafixkidgray">Grafixkid Gray</option>
            <option value="grafixkidgreen">Grafixkid Green</option>
            <option value="blackzero">Game Boy (Black Zero) palette</option>
          </Select>
        </div>
        <HexInput v-model="dumpText" class="h-full" />
        <div class="flex-1 flex justify-end my-2">
          <Button v-if="!isTransfered" :disabled="loading" @click="onImport">Import</Button>
          <Button v-else @click="onRestart">Restart</Button>
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.transparent-image {
  background-image: linear-gradient(45deg, #b0b0b0 25%, transparent 25%),
    linear-gradient(-45deg, #b0b0b0 25%, transparent 25%),
    linear-gradient(45deg, transparent 75%, #b0b0b0 75%),
    linear-gradient(-45deg, transparent 75%, #b0b0b0 75%);
  background-size: 20px 20px;
  background-position: 0 0, 0 10px, 10px -10px, -10px 0px;
}
</style>
