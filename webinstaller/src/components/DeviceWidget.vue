<template>
  <div
    class="radar z-0 top-0 left-0 h-full w-full shrink-0 overflow-hidden absolute"
  ></div>
  <div class="flex justify-center items-center flex-col z-10 mt-20 mb-16 relative">
    <div class="flex flex-row items-center mb-2 md:mb-8 scale-75 md:scale-100">
      <!-- Device -->
      <div class="relative w-[360px] h-[156px] bg-flipper">
        <div
          class="absolute left-[92px] top-[27px] w-[128px] h-[64px] bg-flipper-default"
        ></div>
        <!-- USB PLUG -->
        <div
          class="ml-4 hidden md:block absolute top-[70px] w-[159px] h-[37px] bg-typec right-[-175px]"
        ></div>
      </div>
    </div>

    <!-- TextLabel -->
    <!-- <span
      class="flex flex-1 uppercase text-3xl md:text-5xl z-10 text-primary font-born2bsporty"
      >Install MALVEKE Firmware</span
    > -->
    <InstallButton :_manifest="manifest" text="Install MALVEKE Firmware" @update-state="$emit('update-state', $event)" @log="$emit('log', $event)"></InstallButton>
    <!-- <div
      class="text-lightgreen rounded-md text-2xl md:text-3xl px-12 py-1 border border-lightgreen relative font-born2bsporty uppercase hover:text-white"
      style="background-color: #491d00"
      @click="connectToBluetoothSerial"
    >
      <div
        class="absolute w-full h-full bg-lightgreen opacity-20 hover:opacity-60 hover:cursor-pointer left-0 top-0 z-0"
      ></div>
      <div class="z-10">Permission</div>
    </div> -->
  </div>
</template>
<script>
import InstallButton from "./InstallButton.vue";
export default {
  props: {
    manifest: Object,
  },
  components: {
    InstallButton,
  },
};
</script>
<style lang="scss" scoped>
.radar {
  background-image: linear-gradient(90deg, #491d00 1px, transparent 1px),
    linear-gradient(#491d00 1px, transparent 1px);
  background-size: 27px 27px, 27px 27px, cover, cover;
  &::before {
    @apply absolute h-full w-full z-0 opacity-10;
    content: "";
    animation: spin 4000ms linear infinite;
    background: linear-gradient(transparent, #fe8a2c);
  }
}
@keyframes spin {
  0% {
    transform: translateY(-100%);
  }
  100% {
    transform: translateY(100%);
  }
}
</style>
