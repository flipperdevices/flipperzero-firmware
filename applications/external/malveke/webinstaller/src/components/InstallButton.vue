<template lang="">
  <!-- initializing -->
  <!-- writing -->
  <!-- finished -->
  <!-- error -->
  <button
    v-if="isSupported && isAllowed"
    @click="connectToDevice"
    class="w-[26rem] rounded-md text-2xl md:text-3xl px-12 py-1 border relative font-born2bsporty uppercase"
    :class="{
      'text-lightgreen border-lightgreen hover:text-white': _installState == '',
      'text-lightblue border-lightblue': _installState != '',
    }"
    style="background-color: #491d00"
  >
    <div
      class="absolute bg-lightblue h-full top-0 left-0 opacity-40"
      :style="{
        width: `${_installState?.details?.percentage || 0}%`,
      }"
      style="transition: width 3s ease-in-out;"
    ></div>

    <div
      class="absolute w-full h-full opacity-20 left-0 top-0 z-0"
      :class="{
        'hover:opacity-60 hover:cursor-pointer bg-lightgreen':
          _installState == '',
        'bg-lightblue cursor-auto': _installState != '',
      }"
    ></div>
    <template v-if="_installState == ''">
      {{ text }}
    </template>
    <template v-else>
      {{ _installState?.state }}
    </template>
  </button>
  <div v-if="!isConnected">
    <template v-if="!isAllowed"
      >You can only install ESP devices on HTTPS websites or on the
      localhost.</template
    >
    <template v-if="unsuported">
      >Your browser does not support installing things on ESP devices. Use
      Google Chrome or Microsoft Edge.</template
    >
  </div>
</template>
<script>
import { flash } from "../utils/flash";
import { sleep } from "../utils/sleep";
import { FlashStateType } from "../utils/const.js";
import { msg, logs } from "../store/state";
import { ImprovSerial } from "improv-wifi-serial-sdk/dist/serial";

export default {
  props: {
    _manifest: Object,
    text: String,
  },
  computed: {
    isSupported() {
      return "serial" in navigator;
    },
    isAllowed() {
      return window.isSecureContext;
    },
    unsuported() {
      return !this.isSupported || !this.isAllowed;
    },
  },
  data() {
    return {
      isConnected: false,
      _installState: "",
      manifestPath: "/",
      _state: "DASHBOARD",
      port: null,
      _error: "",
      key: 0,
    };
  },
  methods: {
    _handleDisconnect() {
      this._state = "ERROR";
      this._error = "Disconnected";
    },
    async _initialize(justInstalled = false) {
      if (this.port.readable === null || this.port.writable === null) {
        this._state = "ERROR";
        this._error =
          "Serial port is not readable/writable. Close any other application using it and try again.";
        return;
      }

      if (this._manifest.new_install_improv_wait_time === 0) {
        this._client = null;
        return;
      }

      const client = new ImprovSerial(this.port, {
        log(value) {
          console.log("value", value);
          // this.$emit('log', value)
          logs.update((v) => v.concat(value));
        },
        debug(value) {
          console.log("value", value);
          // this.$emit('log', value)
          logs.update((v) => v.concat(value));
        },
        error(value) {
          console.log("value", value);
          // this.$emit('log', value)
          logs.update((v) => v.concat(value));
        },
      });
      client.addEventListener("state-changed", () => {
        // this.requestUpdate();
        this.key++;
      });
      client.addEventListener("error-changed", () => {
        // this.requestUpdate()
        this.key++;
      });
      try {
        // If a device was just installed, give new firmware 10 seconds (overridable) to
        // format the rest of the flash and do other stuff.
        const timeout = !justInstalled
          ? 1000
          : this._manifest.new_install_improv_wait_time !== undefined
          ? this._manifest.new_install_improv_wait_time * 1000
          : 10000;
        this._info = await client.initialize(timeout);
        this._client = client;
        client.addEventListener("disconnect", this._handleDisconnect);
      } catch (err) {
        console.error(err);
        // Clear old value
        this._info = undefined;
        if (err instanceof PortNotReady) {
          this._state = "ERROR";
          this._error =
            "Serial port is not ready. Close any other application using it and try again.";
        } else {
          this._client = null; // not supported
          console.error("Improv initialization failed.", err);
        }
      }
    },
    async connectToDevice() {
      try {
        this.port = await navigator.serial.requestPort();
        if (!this.port) {
          return;
        }

        try {
          console.log("this.port", this.port);
          await this.port.open({ baudRate: 115200 });
          this.isConnected = true;
        } catch (err) {
          console.error(err);
          this.isConnected = false;
          alert(err.message);
          return;
        }

        // Close port. ESPLoader likes opening it.
        await this.port.close();
        flash(
          (state) => {
            msg.set(state.message);
            this.$emit("update-state", state);
            this._installState = state;

            if (state.state === FlashStateType.FINISHED) {
              sleep(100)
                // Flashing closes the port
                .then(() => this.port.open({ baudRate: 115200 }))
                .then(() => this._initialize(true))
                .then(() => {
                  // this.requestUpdate()
                  this.key++;
                });
            } else if (state.state === FlashStateType.ERROR) {
              sleep(100)
                // Flashing closes the port
                .then(() => this.port.open({ baudRate: 115200 }));
            }
          },
          this.port,
          this.manifestPath,
          this._manifest,
          this._installErase
        );
      } catch (err) {
        if (err.name === "NotFoundError") {
          //   import("./no-port-picked/index").then((mod) =>
          //     mod.openNoPortPickedDialog(() => this.connectToDevice())
          //   );
        } else {
          alert(`Error: ${err.message}`);
        }
      }
    },
  },
};
</script>
<style lang=""></style>
