<script setup>
import Dialog from "./components/Dialog.vue";
import DeviceWidget from "./components/DeviceWidget.vue";
import { computed, ref } from "vue";

// const manifest = {
//   name: "[MALVEKE] Firmware",
//   version: "v0.0.1",
//   builds: [
//     {
//       chipFamily: "ESP32-S2",
//       parts: [
//         { "path": "2_5_1/esp32_marauder.ino.bootloader.bin", "offset": 0x1000 },  
//         { "path": "2_5_1/esp32_marauder.ino.partitions.bin", "offset": 0x8000 },
//         { "path": "2_5_1/boot_app0.bin", "offset": 0xE000 },  //  otadata
//         { "path": "2_5_1/MALVEKE.ino.bin", "offset": 0x10000 }, //  app0
//         {
//           "path": "2_5_1/esp32_marauder.spiffs.bin",
//           "offset": 0x00310000
//         }
//       ],
//     },
//   ],
// };
const message = ref("");
const state = ref("");
const version = ref('2.5.1');

const versionPath = computed(()=> version.value.replace(/\./, '_'));
const manifest = computed(()=> {
  return {
  name: "[MALVEKE] Firmware",
  version: "v0.0.1",
  builds: [
    {
      chipFamily: "ESP32-S2",
      parts: [
        { "path": `${versionPath.value}/esp32_marauder.ino.bootloader.bin`, "offset": 0x1000 },  
        { "path": `${versionPath.value}/esp32_marauder.ino.partitions.bin`, "offset": 0x8000 },
        { "path": `${versionPath.value}/boot_app0.bin`, "offset": 0xE000 },  //  otadata
        { "path": `${versionPath.value}/MALVEKE.ino.bin`, "offset": 0x10000 }, //  app0
        {
          "path": `${versionPath.value}/esp32_marauder.spiffs.bin`,
          "offset": 0x00310000
        }
      ],
    },
  ],
};
});
function onUpdateState(event) {
  message.value = event.message;
  state.value = event.state;
}
function onLog() {

}
</script>

<template>
  <div
    class="opacity-20 w-full h-full top-0 left-0 absolute bg-repeat bg-[length:412.5px_749.25px] pattern"
  ></div>
  <div class="bg-black">
    <div class="absolute w-full h-full top-0 left-0 z-20">
      <Dialog :state="state" :message="message" v-model:version="version">
        <DeviceWidget :manifest="manifest" @update-state="onUpdateState" @log="onLog"></DeviceWidget>
      </Dialog>

    </div>
    <canvas id="confetti-canvas"></canvas>
  </div>
</template>

<style scoped>
.logo {
  height: 6em;
  padding: 1.5em;
  will-change: filter;
  transition: filter 300ms;
}
.logo:hover {
  filter: drop-shadow(0 0 2em #646cffaa);
}
.logo.vue:hover {
  filter: drop-shadow(0 0 2em #42b883aa);
}
</style>
