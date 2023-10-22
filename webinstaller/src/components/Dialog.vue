<template>
  <div class="flex w-full h-full justify-center items-center px-8 py-6">
    <!-- [card] -->
    <div
      class="flex flex-col w-full lg:w-3/6 rounded-lg border-2 px-3 pb-3 bg-black border-flprborder"
    >
      <!-- [header] -->
      <div class="flex justify-between items-center h-11">
        <div class="flex flex-row">
          <div class="rounded-full w-5 h-5 bg-close"></div>
          <div class="rounded-full w-5 h-5 ml-3 bg-minimize"></div>
        </div>
        <div></div>
      </div>
      <!-- [/header] -->
      <div
        class="flex flex-1 flex-col justify-center items-center border-4 border-2 rounded-lg relative overflow-hidden border-primary text-white"
      >
        <slot />
      </div>

      <div class="flex flex-1 w-full mt-3">
        <div class="flex-none mr-3">
          <div
            class="cursor-pointer flex items-center text-primary rounded-md text-3xl md:text-3xl px-4 py-1 border-2 border-primary relative font-haxrcorp-4089 uppercase"
            style="background-color: #442007"
            @click="toggleLogs"
          >
            <div
              class="absolute w-full h-full bg-primary opacity-10 hover:opacity-60 hover:cursor-pointer left-0 top-0 z-1"
            ></div>

            <ArrowUp v-if="!showLogs" />
            <ArrowDown v-else />
            <div class="z-10 ml-2">LOGS</div>
          </div>
        </div>
        <div
          class="flex items-center bg-red-400 flex-1 uppercase text-3xl md:text-3xl z-10 text-primary font-haxrcorp-4089"
          style="background-color: #442007"
        >
          <Info class="ml-2" /> <span class="py-1 px-3">{{ message }}</span>
        </div>
      </div>

      <div id="logs" class="logs w-full mt-3" :class="{
        'hidden': showLogs
      }">
        <div
          class="rounded-lg border-2 px-3 pb-3 bg-black border-flprborder h-44 p-2"
        >
          <div id="terminal">
          </div>
        </div>
      </div>
    </div>
    <!-- [/card] -->
  </div>
</template>
<script>
import Info from "./icons/Info.vue";
import ArrowUp from "./icons/ArrowUp.vue";
import ArrowDown from "./icons/ArrowDown.vue";
import { msg, logs } from "../store/state";
export default {
  components: {
    Info,
    ArrowUp,
    ArrowDown,
  },
  computed: {
    message() {
      return msg();
    },
    logs() {
      return logs();
    },
  },
  data() {
    return {
      showLogs: true,
    };
  },
  methods: {
    toggleLogs() {
      this.showLogs = !this.showLogs
    }
  }
};
</script>
<style lang=""></style>
