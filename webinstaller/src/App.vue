<script setup>
import Dialog from "./components/Dialog.vue";
import DeviceWidget from "./components/DeviceWidget.vue";
import { ref } from "vue";

const manifest = {
  name: "[MALVEKE] Firmware",
  version: "v0.0.1",
  builds: [
    {
      chipFamily: "ESP32-S2",
      parts: [
        { path: "esp32_marauder.ino.bootloader.bin", offset: 4096 },
        { path: "esp32_marauder.ino.partitions.bin", offset: 32768 },
        { path: "boot_app0.bin", offset: 57344 },
        { path: "MALVEKE.ino.bin", offset: 65536 },
      ],
    },
  ],
};
const message = ref("");
const state = ref("");
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
      <Dialog :state="state" :message="message">
        <DeviceWidget :manifest="manifest" @update-state="onUpdateState" @log="onLog"></DeviceWidget>
      </Dialog>
    </div>
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
